# 2D计算单元(SAU)的算子指令表
## 下表为8bit*8bit的算子指令：
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>8bit*8bit算子表格</title>
    <style>
        table {
            width: 100%;
            color: #333;
            border-collapse: collapse; /* Added for better border rendering */
        }
        th, td {
            border: 1px solid #c0c0c0;
            vertical-align: middle; /* 上下居中 */
            text-align: center;    /* 左右居中 */
            padding: 8px; /* 增加一些内边距，使内容不紧贴边框 */
        }
        th {
            background-color: #e0f0ff; /* Light blue header as in the image */
            white-space: nowrap; /* Prevent header text from wrapping too much */
        }
        td:nth-child(1) { /* First column (8bit*8bit算子) */
            text-align: center;
            font-weight: bold;
            white-space: nowrap;
        }
        td:nth-child(2) { /* SAU指令配置 */
            width: 20%;
        }
        td:nth-child(3) { /* Prepare data */
            width: 20%;
        }
        td:nth-child(4) { /* 单个flow内 */
            width: 30%;
        }
        td:nth-child(5) { /* 输出结果方阵 */
            text-align: center;
            width: 15%;
            white-space: nowrap;
        }
    </style>
</head>
<body>

<table>
    <thead>
        <tr>
            <th>8bit*8bit算子</th>
            <th>SAU指令配置(shift_mode: 0)</th>
            <th>Prepare data</th>
            <th>单个flow内</th>
            <th>输出结果方阵(128bit)</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>矩阵乘法 (A*B)</td>
            <td>
                transpose_mode: 1<br>
                conv_kernel: 0<br>
                register_mode: 0<br>
                flow_mode: 0<br>的8个输出通道对应的
                work_mode: 0<br>
                stride_mode: 0
            </td>
            <td>
                矩阵A的8行所有列<br>
                矩阵B的8列所有行
            </td>
            <td>
                Horizon是矩阵A的8行*8*8方阵, 转置输入, 因此每一拍实际输入是该方阵的每一列<br>
                Vertical是矩阵B对应位置的列*8*8方阵, 每一拍输入的是该方阵的一行
            </td>
            <td>结果矩阵的8*8方阵</td>
        </tr>
        <tr>
            <td>PW卷积</td>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 1<br>
                register_mode: 0<br>
                flow_mode: 1<br>
                work_mode: 2<br>
                stride_mode: 0
            </td>
            <td>
                特征图8个点对应的所有输入通道<br>
                卷积核的8个输出通道对应的所有输入通道
            </td>
            <td>
                Horizon是特征图8个通道的8个点(8*8), 每一拍实际输入是一个通道的8个点<br>
                Vertical是对应位置的列*8*8方阵, 8个输出通道的8个输入通道(8*8)
            </td>
            <td>
                8个输出通道<br>
                每个通道8个数
            </td>
        </tr>
        <tr>
            <td rowspan="2">标准卷积</td>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 0<br>
                flow_mode: 输入通道小于8为1, 大于8需判断<br>
                work_mode: 2<br>
                stride_mode: 0<br>
                flow_times: 8
            </td>
            <td>
                stride_mode 0<br>
                特征图8个通道的对应3行, 每行10个数。<br>
                8个输出通道卷积核的8个输入通道的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图每个通道的3行, 每一行复用3次(8*9), 复用方式: (1:1-8,2:2-9,3:3-10)<br>
                Vertical是特征图对应通道的8个输出通道的9个点(8*9), 每一拍输入每个输出通道的对应点
            </td>
            <td>
                8个输出通道<br>
                每个通道8个数
            </td>
        </tr>
        <tr>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 0<br>
                flow_mode: 输入通道小于4为1, 大于4需判断<br>
                work_mode: 2<br>
                stride_mode: 1<br>
                flow_times: 4
            </td>
            <td>
                stride_mode 1<br>
                特征图4个通道的对应3行, 每行18个数。<br>
                8个输出通道卷积核的4个输入通道的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图每个通道的3行, 每一行复用3次(8*9), 复用方式: (1:1-3,15:2-4-16,3:3-5-17)<br>
                Vertical是特征图对应通道的8个输出通道的9个点(8*9), 每一拍输入8个输出通道的对应点
            </td>
            <td>
                8个输出通道<br>
                每个通道一行8个数
            </td>
        </tr>
        <tr>
            <td rowspan="2">DW卷积</td>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 2<br>
                flow_mode: 1<br>
                work_mode: 2<br>
                stride_mode: 0<br>
                flow_times: 8
            </td>
            <td>
                特征图单个通道的对应10行,每行10个数。<br>
                对应通道卷积核的的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图通道内的3行, 每一行复用3次(8*9), 第一次是1-3行, 第二次2-4行, 第8次8-10行<br>
                Vertical是特征图对应通道的卷积核的9个点(8*9)，每一次flow的输入相同
            </td>
            <td>
                单个输出通道<br>
                单个通道8行8个数
            </td>
        </tr>
        <tr>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 2<br>
                flow_mode: 1/2<br>
                work_mode: 2<br>
                stride_mode: 1<br>
                flow_times: 4
            </td>
            <td>
                特征图单个通道的对应9行, 每行18个数。<br>
                对应通道卷积核的的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图通道内的3行, 每一行复用3次(8*9), 第一次是1-3行, 第二次3-5行, 第8次15-17行<br>
                Vertical是特征图对应通道的卷积核的9个点(8*9)，每一次flow的输入相同
            </td>
            <td>
                单个输出通道<br>
                单个通道8行8个数
            </td>
        </tr>
    </tbody>
</table>

</body>
</html>

## 下表为16bit*8bit的算子指令：
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>8bit*8bit算子表格</title>
    <style>
        table {
            width: 100%;
            color: #333;
            border-collapse: collapse; /* Added for better border rendering */
        }
        th, td {
            border: 1px solid #c0c0c0;
            vertical-align: middle; /* 上下居中 */
            text-align: center;    /* 左右居中 */
            padding: 8px; /* 增加一些内边距，使内容不紧贴边框 */
        }
        th {
            background-color: #e0f0ff; /* Light blue header as in the image */
            white-space: nowrap; /* Prevent header text from wrapping too much */
        }
        td:nth-child(1) { /* First column (8bit*8bit算子) */
            text-align: center;
            font-weight: bold;
            white-space: nowrap;
        }
        td:nth-child(2) { /* SAU指令配置 */
            width: 20%;
        }
        td:nth-child(3) { /* Prepare data */
            width: 20%;
        }
        td:nth-child(4) { /* 单个flow内 */
            width: 30%;
        }
        td:nth-child(5) { /* 输出结果方阵 */
            text-align: center;
            width: 15%;
            white-space: nowrap;
        }
    </style>
</head>
<body>

<table>
    <thead>
        <tr>
            <th>8bit*8bit算子</th>
            <th>SAU指令配置(shift_mode: 0)</th>
            <th>Prepare data</th>
            <th>单个flow内</th>
            <th>输出结果方阵(128bit)</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>矩阵乘法 (A*B)</td>
            <td>
                transpose_mode: 1<br>
                conv_kernel: 0<br>
                register_mode: 0<br>
                flow_mode: 0<br>
                work_mode: 0<br>
                stride_mode: 0
            </td>
            <td>
                矩阵A的8行所有列<br>
                矩阵B的8列所有行
            </td>
            <td>
                Horizon是矩阵A的行8*8方阵,转置输入，来自于相同输入的16bit低8bit和高8bit组成的64bit，在连续两个flow依次切换<br>
                Vertical是矩阵B对应位置的列8*8方阵，该8*8方阵在连续两个flow内是相同的
            </td>
            <td>结果矩阵的8*8方阵</td>
        </tr>
        <tr>
            <td>PW卷积</td>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 1<br>
                register_mode: 0<br>
                flow_mode: 1<br>
                work_mode: 2<br>
                stride_mode: 0
            </td>
            <td>
                特征图8个点对应的所有输入通道<br>
                卷积核的8个输出通道对应的所有输入通道
            </td>
            <td>
                Horizon是特征图8个通道的8个点(8*8)，每一拍实际输入是一个通道的8个点，来自于相同输入的16bit低8bit和高8bit组成的64bit，在连续两个flow依次切换<br>
                Vertical是对应位置的列8*8方阵，8个输出通道的8个输入通道(8*8)，该8*8方阵在连续两个flow内是相同的
            </td>
            <td>
                8个输出通道<br>
                每个通道8个数
            </td>
        </tr>
        <tr>
            <td rowspan="2">标准卷积</td>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 0<br>
                flow_mode: 输入通道小于4为1, 大于4需判断<br>
                work_mode: 2<br>
                stride_mode: 0<br>
                flow_times: 4
            </td>
            <td>
                stride_mode 0<br>
                特征图4个通道的对应3行,每行10个数。<br>
                8个输出通道卷积核的4个输入通道的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图每个通道的3行，每一行复用3次(8*9)，复用方式（1:1-8,2:2-9,3:3-10）。若当前flow复用通道的3行低位，则下一个flow复用该通道的高位<br>
                Vertical是特征图对应通道的8个输出通道的9个点(8*9)，每一拍输入8个输出通道的对应点，在连续两个flow内是相同的
            </td>
            <td>
                8个输出通道<br>
                每个通道8个数
            </td>
        </tr>
        <tr>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 0<br>
                flow_mode: 输入通道小于2为1, 大于2需判断<br>
                work_mode: 2<br>
                stride_mode: 1<br>
                flow_times: 2
            </td>
            <td>
                stride_mode 1<br>
                特征图2个通道的对应3行, 每行18个数。<br>
                8个输出通道卷积核的2个输入通道的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图每个通道的3行，每一行复用3次(8*9)，复用方式（1:1,3-15,2:2,4-16,3:3,5-17）。若当前flow复用通道的3行低位，则下一个flow复用该通道的高位<br>
                Vertical是特征图对应通道的8个输出通道的9个点(8*9),每一拍输入8个输出通道的对应点，在连续两个flow内是相同的
            </td>
            <td>
                8个输出通道<br>
                每个通道一行8个数
            </td>
        </tr>
        <tr>
            <td rowspan="2">DW卷积</td>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 2<br>
                flow_mode: 1<br>
                work_mode: 2<br>
                stride_mode: 0<br>
                flow_times: 8
            </td>
            <td>
                stride_mode 0<br>
                特征图单个通道的对应10行,每行10个数。<br>
                对应通道卷积核的的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图通道内的3行，每一行复用3次(8*9)，第一次是1-3行，第二次是2-4行，第8次8-10行。若当前flow复用通道的3行低位，则下一个flow复用该通道的高位<br>
                Vertical是特征图对应通道的8个输出通道的9个点(8*9)，每一次flow的输入相同
            </td>
            <td>
                单个输出通道<br>
                单个通道8行8个数
            </td>
        </tr>
        <tr>
            <td>
                transpose_mode: 0<br>
                conv_kernel: 3<br>
                register_mode: 2<br>
                flow_mode: 1/2<br>
                work_mode: 2<br>
                stride_mode: 1<br>
                flow_times: 2
            </td>
            <td>
                stride_mode 1 <br>
                特征图单个通道的对应5行, 每行18个数。<br>
                对应通道卷积核的的9个数 (3*3)
            </td>
            <td>
                Horizon是特征图通道内的3行，每一行复用3次(8*9)，第一次是1-3行，第二次是3-5行，第8次15-17行。若当前flow复用通道的3行低位，则下一个flow复用该通道的高位<br>
                Vertical是特征图对应通道的卷积核的9个点(8*9)，每一次flow的输入相同
            </td>
            <td>
                单个输出通道<br>
                单个通道8行8个数
            </td>
        </tr>
    </tbody>
</table>

</body>
</html>