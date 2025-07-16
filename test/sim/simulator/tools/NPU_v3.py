############### Edited by ZW ##############
# v3版本
# 矩阵运算与卷积运算基本逻辑，其中卷积考虑感受野，register width改成18byte，在stride=1 kernel=3时将多余的2bytes存到第17 18位
# 在stride=2 kernel=3时将多余的1bytes存到第二行的第17位

class ins_reg:
    def __init__(self):
        self.A_address = 0
        self.A_address_step1 = 0
        self.A_address_step2 = 0
        self.B_address = 0
        self.B_address_step1 = 0
        self.B_address_step2 = 0
        self.C_address = 0
        self.D_address = 0
        self.D_address_step1 = 0
        self.D_address_step2 = 0
        self.flow_loop_times = 0
        self.work_mode = 0
        self.flow_mode = 0
        self.register_mode = 0
        self.conv_kernel = 0
        self.transpose_mode = 0
        self.stride = 0
        self.start = 0
        self.id = 0

    # 解析指令
    def ins_analyze(self, ins1_bin, ins2_bin, ins3_bin):
        print('------------------------- SAU INS INFO START --------------------------')
        ins1_bin = ins1_bin[::-1]
        # print(ins1_bin)
        ins2_bin = ins2_bin[::-1]
        # print(ins2_bin)
        ins3_bin = ins3_bin[::-1]
        # print(ins3_bin)
        vertical_address = (ins1_bin[:20])[::-1]  # 矩阵A输入数据ram地址, 每个地址存一个byte
        self.A_address = int(vertical_address[:-4], 2)
        vertical_x_step = (ins1_bin[20:25])[::-1]
        self.A_address_step1 = int(vertical_x_step, 2) * 16  # 同一个flow中，存在register中的每一行的数据对应在ram的地址差,以字节为单位
        vertical_ch_step = (ins1_bin[25:32])[::-1]
        self.A_address_step2 = int(vertical_ch_step, 2) * self.A_address_step1  # 不同flow数据的地址差

        horizontal_address = (ins1_bin[32:52])[::-1]  # 矩阵B输入数据ram地址
        self.B_address = int(horizontal_address[:-4], 2)
        horizontal_x_step = (ins1_bin[52:57])[::-1]
        self.B_address_step1 = int(horizontal_x_step, 2) * 16  # 同一个flow中，存在register中的每一行的数据对应在ram的地址差,以字节为单位
        horizontal_ch_step = (ins1_bin[57:63])[::-1]
        self.B_address_step2 = int(horizontal_ch_step, 2) * self.B_address_step1  # 不同flow数据的地址差

        output_address = (ins2_bin[:20])[::-1]
        self.D_address = int(output_address[:-4], 2)
        output_x_step = (ins2_bin[20:25])[::-1]
        self.D_address_step1 = int(output_x_step, 2)
        output_ch_step = (ins2_bin[25:32])[::-1]
        self.D_address_step2 = int(output_ch_step, 2) * self.D_address_step1

        self.flow_loop_times = int((ins2_bin[32:38])[::-1], 2)  # in_channel compute every cycle
        self.work_mode = int((ins2_bin[38:40])[::-1], 2)  # 00为矩阵乘，01为转置，10为矩阵乘+add, 11为add
        self.flow_mode = int((ins2_bin[40:42])[::-1], 2)  # 00正常输出-清零,01转置输出-清零,10正常输出-留存,11转置输出-留存 （指令之间）
        self.register_mode = int((ins2_bin[42:44])[::-1], 2)  # 配合conv kernel计算register addr 00指令结束清零，01卷积复用模式
        self.conv_kernel = int((ins2_bin[44:46])[::-1], 2)
        self.transpose_mode = int((ins2_bin[46:48])[::-1], 2)  # pre，00为不转置，01为A转置，10为B转置
        self.stride = int((ins2_bin[48:50])[::-1], 2)

        self.start = int(ins3_bin[0], 2)
        self.id = int((ins3_bin[32:38])[::-1], 2)
        bias_address = (ins3_bin[40:60])[::-1]
        self.C_address = int(bias_address[:-4], 2)
        print('vertical_address:', vertical_address, 'horizontal_address:', horizontal_address, 'bias_address:',
              bias_address, 'output_address:', output_address)
        print('A_address:', self.A_address, 'B_address:', self.B_address, 'C_address:', self.C_address, 'D_address:',
              self.D_address)
        print('flow_loop_times:', self.flow_loop_times, 'work_mode:', self.work_mode, 'flow_mode:', self.flow_mode,
              'register_mode:', self.register_mode)
        print('conv_kernel:', self.conv_kernel, 'start:', self.start, 'id:', self.id)
        print('-------------------------- SAU INS INFO END ---------------------------')


def transpose(A):
    B = [[A[j][i] for j in range(len(A))] for i in range(len(A[0]))]
    return B


# 十六进制补码的乘法,先用符号位扩展，再计算
def multiply(fm, weight, hexlen_fm, hexlen_w):
    fm_bin = bin(fm)
    fm_str = str(fm_bin)[2:]
    fm_str = fm_str.rjust(hexlen_fm * 4, '0')
    if (fm_str[0] == '1'):
        fm_str = fm_str.rjust((hexlen_fm + hexlen_w) * 4, '1')
    fm_int = int(fm_str, 2)  # 二进制字符串转十进制

    w_bin = bin(weight)
    w_str = str(w_bin)[2:]
    w_str = w_str.rjust(hexlen_w * 4, '0')
    if (w_str[0] == '1'):
        w_str = w_str.rjust((hexlen_fm + hexlen_w) * 4, '1')
    w_int = int(w_str, 2)

    out_int = fm_int * w_int  # 计算出来的十进制，可能会溢出
    out_bin = bin(out_int)[2:]  # 转化成十六进制字符串
    out_str = out_bin.rjust((hexlen_fm + hexlen_w) * 4, '0')
    out_str = out_str[-(hexlen_w + hexlen_fm) * 4:]
    if (out_str[0] == '1'):
        out_str = out_str.rjust(24, '1')
    out = int(out_str, 2)

    return out


# 脉动阵列乘法
def systolic_array(A, B, ins, A_width, B_width):
    work_mode = ins.work_mode
    if (work_mode == 3):
        C = [[0] * len(A[0]) for _ in range(len(A))]
        for i in range(len(A)):
            for j in range(len(A[0])):
                A_bin = bin(A[i][j])
                A_str = str(A_bin)[2:]
                A_str = A_str.rjust(A_width, '0')
                if (A_str[0] == '1'):
                    A_str = A_str.rjust(24, '1')
                A[i][j] = int(A_str, 2)

                B_bin = bin(B[i][j])
                B_str = str(B_bin)[2:]
                B_str = B_str.rjust(B_width, '0')
                if (B_str[0] == '1'):
                    B_str = B_str.rjust(24, '1')
                B[i][j] = int(B_str, 2)

                C[i][j] = A[i][j] + B[i][j]
    else:
        # 确保矩阵维度匹配
        if len(A[0]) != len(B):
            raise ValueError("矩阵A的列数必须等于矩阵B的行数才能相乘")

        # 初始化结果矩阵C，大小为 A的行数 x B的列数
        C = [[0] * len(B[0]) for _ in range(len(A))]

        # 矩阵相乘
        for i in range(len(A)):  # A的行数
            for j in range(len(B[0])):  # B的列数
                for k in range(len(B)):  # A的列数 / B的行数
                    C[i][j] += multiply(A[i][k], B[k][j], 2, 2)

    return C


# 将输入的txt数据存到数组里
def txt2ram(file, memory_ram, A_width_byte, B_width_byte, C_width_byte, memory_ram_A, memory_ram_B, memory_ram_C):
    """
    此函数可以将输入的文件读入并转换为内存数组格式
    """
    ramfile = open(file, 'r')
    ram = [line.strip("\n") for line in ramfile]
    ram_all = ''
    A_width_hex = A_width_byte * 2
    B_width_hex = B_width_byte * 2
    C_width_hex = C_width_byte * 2
    for i in range(len(ram)):
        ram_all += ram[i]
        for j in range(len(memory_ram[i])):
            memory_ram[i][j] = int(ram[i][j * 2:j * 2 + 2], 16)

    for k in range(int(len(ram_all) / A_width_hex)):
        memory_ram_A[k] = int(ram_all[A_width_hex * k:A_width_hex * k + A_width_hex], 16)
    for k in range(int(len(ram_all) / B_width_hex)):
        memory_ram_B[k] = int(ram_all[B_width_hex * k:B_width_hex * k + B_width_hex], 16)
    for k in range(int(len(ram_all) / C_width_hex)):
        memory_ram_C[k] = int(ram_all[C_width_hex * k:C_width_hex * k + C_width_hex], 16)
        # 给bias补成24bit
        memory_ram_C_bin = bin(memory_ram_C[k])
        memory_ram_C_str = str(memory_ram_C_bin)[2:]
        memory_ram_C_str = memory_ram_C_str.rjust(C_width_hex * 4, '0')
        if (memory_ram_C_str[0] == '1'):
            memory_ram_C_str = memory_ram_C_str.rjust(24, '1')
        memory_ram_C[k] = int(memory_ram_C_str, 2)
    return memory_ram, memory_ram_A, memory_ram_B, memory_ram_C


def AB_Prepocess(flow_i, memory_ram_A, memory_ram_B, register, A_matrix, B_matrix, register_empty, register_wr_row,
                 register_rd_row, ins, config):
    A_address = ins.A_address
    A_address_step1 = ins.A_address_step1
    A_address_step2 = ins.A_address_step2
    B_address = ins.B_address
    B_address_step1 = ins.B_address_step1
    B_address_step2 = ins.B_address_step2
    flow_loop_times = ins.flow_loop_times
    work_mode = ins.work_mode
    transpose_mode = ins.transpose_mode
    register_mode = ins.register_mode
    conv_kernel = ins.conv_kernel
    stride = ins.stride

    memory_ram_width = config.memory_ram_width
    memory_ram_depth = config.memory_ram_depth
    A_width_byte = config.A_width_byte
    B_width_byte = config.B_width_byte
    transpose_module_width = config.transpose_module_width
    transpose_module_depth = config.transpose_module_depth
    systolic_array_width = config.systolic_array_width
    systolic_array_depth = config.systolic_array_depth
    register_width = config.register_width
    register_depth = config.register_depth
    A_matrix_width = config.A_matrix_width
    A_matrix_depth = config.A_matrix_depth
    B_matrix_depth = config.B_matrix_depth
    ram_load_time_A = config.ram_load_time_A
    ram_load_time_B = config.ram_load_time_B

    # memory_ram_A的起始地址
    A_base_address = A_address * int(memory_ram_width / A_width_byte)
    B_base_address = B_address * int(memory_ram_width / B_width_byte)
    transpose_input = [[0 for i in range(transpose_module_width)] for j in range(transpose_module_depth)]

    # work_mode为矩阵乘或conv模式
    if (work_mode == 0 or work_mode == 2):
        conv_flag = 0 if conv_kernel == 0 else 1
        ################################ 卷积模式，A矩阵为权重矩阵，转置模块的输出；B为特征图矩阵，register的输出 ##############################
        if (conv_flag):
            ################################# A矩阵转置 ########################################
            # 不同flow之间的地址差
            base_address = A_base_address + flow_i * int(A_address_step2 / A_width_byte)
            for j in range(transpose_module_depth):
                # 同一flow不同行的地址差
                if (flow_loop_times == 1):
                    tmp_address = base_address + j * int(A_address_step2 / A_width_byte)
                else:
                    tmp_address = base_address + j * int(A_address_step1 / A_width_byte)

                for k in range(transpose_module_width):
                    transpose_input[j][k] = memory_ram_A[tmp_address]
                    tmp_address += 1
                ram_load_time_A += 1
            # A矩阵做转置
            if (transpose_mode == 1 or transpose_mode == 3):
                A_matrix = transpose(transpose_input)
            else:
                A_matrix = transpose_input

            ################################ 存register file,给B矩阵赋值 #######################################
            # 不同flow之间的地址差
            base_address = B_base_address + flow_i * int(B_address_step2 / B_width_byte)

            if register_mode == 0 and register_empty[flow_i] == 0:  # 列方向复用结束，需要清零
                register_empty[flow_i] = 1

            # FIXME:list index out of range
            if register_empty[flow_i] == 1:
                if flow_loop_times == 1:
                    wr_times = 16
                else:
                    wr_times = conv_kernel
                register_rd_row[flow_i] = int(register_depth / flow_loop_times) * flow_i
            elif register_mode == 1:
                wr_times = 1

            for ck in range(wr_times):
                # 记录每个flow的register的将来的存取地址（register被分成了flow_loop_times组）
                if register_empty[flow_i] == 1:
                    register_empty[flow_i] = 0
                    register_wr_row[flow_i] = int(register_depth / flow_loop_times) * flow_i
                else:
                    if register_wr_row[flow_i] == int(register_depth / flow_loop_times) * (flow_i + 1) - 1:
                        register_wr_row[flow_i] = int(register_depth / flow_loop_times) * flow_i
                    else:
                        register_wr_row[flow_i] += 1
                # 同一flow不同行的地址差
                if flow_loop_times == 1:
                    tmp_address = base_address + ck * int(B_address_step2 / B_width_byte)
                else:
                    tmp_address = base_address + ck * int(B_address_step1 / B_width_byte)
                # 判断是否需要读32个数用于给B补齐,且stride=2的时候需要读32+16=48个数
                if stride == 1 and flow_loop_times != 1:
                    if (conv_kernel == 3):
                        num_row = 1
                        width_flag = 1
                    else:
                        num_row = 2
                        width_flag = 0
                elif stride == 2 and flow_loop_times != 1:
                    if (conv_kernel == 3):
                        num_row = 2
                        width_flag = 1
                    else:
                        num_row = 3
                        width_flag = 0
                else:
                    num_row = 1
                    width_flag = 0
                for n in range(num_row):
                    if n == num_row - 1 and width_flag == 1:
                        register_wr_width = register_width
                    else:
                        register_wr_width = systolic_array_width
                    for k in range(register_wr_width):
                        # 把register分成 register_depth/flow_loop_times 段，可以保证每一段的特征图在H维度是连续的存储
                        register[register_wr_row[flow_i]][k] = memory_ram_B[tmp_address]
                        tmp_address += 1
                    # print(flow_i,':',register_wr_row[flow_i],':',register[register_wr_row[flow_i]][:])
                    if (num_row != 1 and n < num_row - 1):
                        register_wr_row[flow_i] += 1
                    ram_load_time_B += 1

            # flow_loop_times==1时，特征图一个flow内读取第一个输入通道的16个像素后往输入通道循环
            if flow_loop_times == 1:
                register_wr_width = systolic_array_width
                for n in range(16):
                    for z in range(register_wr_width):
                        B_matrix[z][n] = register[register_rd_row[flow_i]][z]
                    if register_rd_row[flow_i] == int(register_depth / flow_loop_times) * (flow_i + 1) - 1:
                        register_rd_row[flow_i] = int(register_depth / flow_loop_times) * flow_i
                    else:
                        register_rd_row[flow_i] += 1
            else:
                register_num_row = [0 for i in range(register_width * num_row)]
                for m in range(conv_kernel):
                    # 把register的两行合并成一行，便于给B赋值
                    temp_idex = 0
                    for r in range(num_row):
                        if r == num_row - 1 and width_flag == 1:
                            register_wr_width = register_width
                        else:
                            register_wr_width = systolic_array_width
                        for w in range(register_wr_width):
                            register_num_row[temp_idex] = register[register_rd_row[flow_i]][w]
                            temp_idex += 1
                        # print(flow_i,':',register_rd_row[flow_i],':',register_num_row,register[register_rd_row[flow_i]])
                        if register_rd_row[flow_i] == int(register_depth / flow_loop_times) * (flow_i + 1) - 1:
                            register_rd_row[flow_i] = int(register_depth / flow_loop_times) * flow_i
                        else:
                            register_rd_row[flow_i] += 1
                    # print(register_2_row)
                    for n in range(conv_kernel):
                        for z in range(B_matrix_depth):
                            if stride == 1:
                                B_matrix[z][conv_kernel * m + n] = register_num_row[n + z]
                            elif stride == 2:
                                B_matrix[z][conv_kernel * m + n] = register_num_row[n + 2 * z]

            # 对于conv_kernel>1时，下次会复用前conv_kernel-1行数据，故把读取地址往前调
            if conv_kernel > 1:
                for m in range(conv_kernel - 1):
                    if register_rd_row[flow_i] == int(register_depth / flow_loop_times) * flow_i:
                        register_rd_row[flow_i] = int(register_depth / flow_loop_times) * (flow_i + 1) - num_row
                    else:
                        register_rd_row[flow_i] -= num_row
        #################################  普通矩阵乘模式  #########################################################
        else:
            #################### A矩阵 ###################
            # 不同flow之间的地址差
            base_address = A_base_address + flow_i * int(A_address_step2 / A_width_byte)
            for j in range(A_matrix_depth):
                # 同一flow不同行的地址差
                tmp_address = base_address + j * int(A_address_step1 / A_width_byte)
                for k in range(A_matrix_width):
                    A_matrix[j][k] = memory_ram_A[tmp_address]
                    tmp_address += 1
            ################### B矩阵转置 ################
            # 不同flow之间的地址差
            base_address = B_base_address + flow_i * int(B_address_step2 / B_width_byte)
            for j in range(transpose_module_depth):
                # 同一flow不同行的地址差
                tmp_address = base_address + j * int(B_address_step1 / B_width_byte)
                for k in range(transpose_module_width):
                    # 此处transpose_input是转置的，因为硬件的B矩阵转置其实是不转置
                    transpose_input[k][j] = memory_ram_B[tmp_address]
                    tmp_address += 1
            # B矩阵做转置
            if (transpose_mode == 2):
                B_matrix = transpose(transpose_input)
            else:
                B_matrix = transpose_input

    # work_mode为矩阵加模式
    elif (work_mode == 3):
        base_address = A_base_address + flow_i * int(A_address_step2 / A_width_byte)
        for j in range(systolic_array_depth):
            # 同一flow不同行的地址差
            tmp_address = base_address + j * int(A_address_step1 / A_width_byte)
            for k in range(systolic_array_width):
                A_matrix[j][k] = memory_ram_A[tmp_address]
                tmp_address += 1
            ram_load_time_A += 1

        base_address = B_base_address + flow_i * int(B_address_step2 / B_width_byte)
        for j in range(systolic_array_depth):
            # 同一flow不同行的地址差
            if transpose_mode == 2:
                tmp_address = base_address + j * int(B_address_step1 / B_width_byte)
            else:
                print("error:矩阵加transpose_mode应为2,而transpose_mode=", transpose_mode)
                exit()
            for k in range(systolic_array_width):
                B_matrix[systolic_array_depth - j - 1][k] = memory_ram_B[tmp_address]

    return A_matrix, B_matrix, register, register_empty, register_wr_row, register_rd_row
