#include "sau_benchmark_func16b.hpp"

void _split_bigmat_128K(int16_t *input_blockA, int8_t *input_blockB,
                     int M, int K, int N,
                     int16_t *newA1, int16_t *newA2,
                     int8_t *newB1, int8_t *newB2) {
  int sizeA = M*K, sizeB = K*N;
  // matrix A横向拆分
  int idx1 = 0, idx2 = 0;
  for (int i = 0; i < sizeA; i++) {
    int block_id = i / (K / 2);
    if (block_id % 2 == 0) {
      newA1[idx1++] = input_blockA[i];
    } else {
      newA2[idx2++] = input_blockA[i];
    }
  }
  // matrix B纵向拆分
  int half = sizeB / 2;
  for (int i = 0; i < half; ++i) {
    newB1[i] = input_blockB[i];
    newB2[i] = input_blockB[i + half];
  }
}

void _split_bigmat_128N(int8_t *input_blockB, int K, int N, int8_t *newB1, int8_t *newB2) {
  int sizeB = K*N;
  // matrix B横向拆分
  int idx1 = 0, idx2 = 0;
  for (int i = 0; i < sizeB; i++) {
    int block_id = i / (N / 2);
    if (block_id % 2 == 0) {
      newB1[idx1++] = input_blockB[i];
    } else {
      newB2[idx2++] = input_blockB[i];
    }
  }
}

void _merge_smallmat_128N(int16_t *newD1, int16_t *newD2, int M, int N, int16_t *output_blockD) {
  int sizeD = M*N;
  int idx1 = 0, idx2 = 0;
  // matrix D横向合并
  for (int i = 0; i < sizeD; i++) {
    int block_id = i / (N / 2);
    if (block_id % 2 == 0) {
      output_blockD[i] = newD1[idx1++];
    } else {
      output_blockD[i] = newD2[idx2++];
    }
  }
}

static const int16_t D_split_0_sa[512] = {
    -145,-10,96,-104,-24,-15,-126,-232,-64,70,223,-333,-127,259,-75,417,223,-85,282,78,61,-58,228,-61,-8,-488,559,-321,256,-227,328,58,36,-43,-269,-258,104,246,15,135,227,-26,-239,-208,194,152,-319,-137,198,154,154,211,23,-101,371,4,144,-406,-22,-112,-235,92,-34,-117,
    186,197,45,269,-128,7,-48,123,18,-354,224,4,-123,305,-100,127,-57,67,67,22,230,-90,-101,84,-237,146,-120,-142,228,-30,432,242,-70,-166,162,-256,-81,88,-151,170,-109,73,-118,37,-228,255,509,-165,-3,234,-60,-252,165,95,403,141,3,-79,279,-79,444,128,41,148,
    -80,343,526,-377,141,161,198,-556,114,-74,88,-34,-299,269,63,639,250,-485,12,-341,112,337,-71,-235,-205,-708,349,-740,270,-236,215,130,226,239,-249,20,12,577,135,203,-24,8,-497,-522,-248,-319,-43,-70,292,275,-43,-79,-222,-224,303,-415,-363,-617,145,-64,-22,-24,-303,-286,
    -442,17,20,-411,-104,-111,-284,-227,-187,353,-117,-100,155,-130,-13,159,-116,-17,-327,-180,-139,54,389,149,151,-404,487,96,-94,-179,-251,-179,260,-153,-541,238,76,109,-232,-120,190,-44,-212,-33,182,-132,-401,8,55,-319,88,347,8,-348,-112,54,63,-202,-303,137,-437,83,195,-78,
    -287,-266,-304,-325,231,342,351,-173,221,69,-172,233,275,-286,-1,-107,280,-56,-18,-161,-187,45,18,-62,405,-377,-64,205,-150,-64,-326,140,-145,527,-178,297,21,-107,302,33,285,-157,-27,-169,125,-265,-550,110,343,-50,-221,200,-505,122,-270,-216,-384,-53,-371,-181,-528,-266,-304,24,
    25,-103,-332,110,-241,-336,-456,435,62,-212,-110,100,-1,-273,-103,-456,-408,122,-347,104,-178,-491,344,204,-144,186,92,609,80,242,-81,133,101,-448,-230,-110,-32,-129,-561,34,-163,-179,158,387,111,372,200,-135,-272,-514,-202,201,301,84,-323,403,39,174,72,73,342,584,533,342,
    235,-441,-368,107,341,237,460,124,466,-261,-67,202,-61,-266,-3,-393,398,-136,432,239,-198,-251,-194,-365,158,-54,-291,183,46,270,-69,280,-343,564,281,-104,-7,-224,497,192,70,-202,347,-139,164,49,-374,76,162,113,-291,46,-404,538,-327,-187,-361,125,-47,-294,-111,-118,-350,101,
    -339,264,198,-47,-460,-208,-312,-76,-591,367,62,-110,52,80,21,432,-373,111,-389,-261,172,105,202,314,62,157,266,-134,-315,-189,-34,-472,177,-552,-220,117,-25,71,-374,-58,-35,233,-258,33,-157,134,175,-199,-339,-24,295,-217,265,-484,414,275,276,125,-277,362,-40,-72,289,53
};
static const int16_t D_split_1_sa[512] = {
    25,-44,166,297,136,150,-404,43,47,568,404,-279,288,354,-291,40,111,-160,459,-206,-641,-266,-45,-343,-76,-96,-43,-529,173,-290,-236,142,195,-263,-11,154,-103,440,159,2,-586,-225,-462,-61,233,-94,80,-193,-72,138,-503,-11,-60,225,-209,64,-446,139,163,-58,-163,410,370,332,
    -122,178,-282,42,-143,-72,124,-97,-261,121,142,-391,67,-272,-197,118,-22,30,-155,102,-68,-177,-203,27,-83,-202,-98,-337,-156,-321,9,-75,-136,95,59,-144,-122,60,9,44,135,29,58,-225,-12,108,-272,-162,-55,-79,-69,177,-13,312,-34,-289,-52,-318,238,-360,-72,169,213,83,
    445,-357,-149,17,50,-29,-368,-180,387,687,295,-295,502,-7,-225,-75,221,39,324,-202,-563,8,-430,-332,-619,-116,313,-349,405,-429,29,-82,305,-396,215,-38,352,294,248,-100,-776,-206,-516,198,30,-212,367,-167,-484,356,-579,-131,46,369,-352,21,-437,679,145,346,-147,7,-30,72,
    141,-190,432,28,245,146,-288,109,236,285,126,70,109,328,50,-28,100,-20,518,-142,-233,-134,-2,35,-132,335,-4,199,474,156,-9,156,127,8,14,25,66,255,74,-93,-353,-4,-243,241,215,12,245,365,122,18,-287,-172,-277,-34,201,112,15,333,-334,168,-133,118,32,148,
    442,-527,178,113,342,339,-118,-83,307,-67,-171,210,-146,314,10,-83,14,-147,236,-138,89,6,173,-200,85,-3,195,208,78,101,68,-126,380,-263,-245,359,-79,-77,40,210,-260,-112,-140,304,-204,-326,321,-17,-384,19,-120,-290,-125,65,-33,390,-253,337,-61,447,-52,53,-358,41,
    -482,454,96,212,270,97,28,-182,-142,-297,-365,-20,-332,104,125,-207,92,325,20,175,384,-220,262,512,168,381,-166,83,234,236,372,93,-322,354,-43,-6,-39,-97,-252,-18,365,171,285,-78,38,-7,-450,430,77,-186,70,-206,-377,25,263,-9,141,-317,-491,-372,144,329,246,273,
    33,-99,-161,281,238,218,-22,-242,125,-394,-404,259,-307,221,-16,-227,-17,-48,-199,-30,297,89,424,-217,330,-195,136,-183,-314,68,143,-120,145,-316,-256,467,-86,-300,-125,240,-4,-183,72,14,-357,-439,-27,-339,-525,96,169,-231,128,-21,-355,427,-420,10,93,210,218,115,-188,61,
    -3,133,109,-283,-88,-107,54,396,-160,276,439,-36,121,-383,-137,522,168,-136,157,-67,-183,-161,-235,372,-80,210,-151,259,111,99,-169,126,-276,526,202,-402,-57,171,160,-46,216,432,65,-86,265,782,-89,389,457,-297,-6,556,-283,-179,426,-572,584,-247,-163,-387,-443,19,269,-258
};


void matmul_multi16(int16_t *input_blockA, int8_t *input_blockB, int16_t *output_blockD, int M, int K, int N) {
  // 1. 前序准备
  const int us=8;
	ACENN_DEBUG("SAU matmul_multi16 START: A[%d,%d] B[%d,%d] -> D[%d,%d]", M, K, K, N, M, N);
  int32_t a,b,c,d,e,f;
  // 2. 判断是否因 h_x wrap而需要拆分
  if ((K == 128) && (N < 128)) {
    int16_t *newA1 = (int16_t*)malloc(M*K/2*sizeof(int16_t));
    int16_t *newA2 = (int16_t*)malloc(M*K/2*sizeof(int16_t));
    int8_t  *newB1 = (int8_t*)malloc(K*N/2*sizeof(int8_t));
    int8_t  *newB2 = (int8_t*)malloc(K*N/2*sizeof(int8_t));
    _split_bigmat_128K(input_blockA, input_blockB, M, K, N, newA1, newA2, newB1, newB2);
    // ACENN_DEBUG("============ input test ============");
    // for(int i = 0;i < M*K/2; ++i){if(newA1[i] != A_split_0_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, A_split_0_sa[i], newA1[i]);}}
    // for(int i = 0;i < M*K/2; ++i){if(newA2[i] != A_split_1_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, A_split_1_sa[i], newA2[i]);}}
    // for(int i = 0;i < K*N/2; ++i){if(newB1[i] != B_split_0_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, B_split_0_sa[i], newB1[i]);}}
    // for(int i = 0;i < K*N/2; ++i){if(newB2[i] != B_split_1_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, B_split_1_sa[i], newB2[i]);}}
    // ACENN_DEBUG("============ input test ============");
    // matmul_multi16(newA1, newB1, output_blockD, M, K/2, N);
    // ACENN_DEBUG("============ output 0 test ============");
    // for(int i = 0;i < M*N/2; ++i){if(output_blockD[i] != D_split_0_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, D_split_0_sa[i], output_blockD[i]);}}
    // ACENN_DEBUG("============ output 0 test ============");
    // matmul_multi16(newA2, newB2, output_blockD, M, K/2, N);
    // ACENN_DEBUG("============ output 1 test ============");
    // for(int i = 0;i < M*N/2; ++i){if(output_blockD[i] != D_split_1_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, D_split_1_sa[i], output_blockD[i]);}}
    // ACENN_DEBUG("============ output 1 test ============");
    int new_M = M;
    int new_K = K/2;
    int new_N = N;
    int32_t h_ch=0,h_x=2*new_K/us,v_ch=us,v_x=new_N/us,o_ch=1,o_x=2*new_N/us;
    int32_t id=0,bias_address=0,flow_loop_times=new_K/us;
    int32_t stride_mode=0,transpose_mode=1;
    int32_t cutbit=5, shift_mode=1;
    for (int i = 0; i < new_M/us; i++) {
      for (int j = 0; j < new_N/us; j++) {
        int32_t h_addr=(int32_t)(uintptr_t)(newA1 + us*new_K*i);
        int32_t v_addr=(int32_t)(uintptr_t)(newB1 + us*j);
        int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(new_N*i+j));
        int32_t conv_kernel=0, register_mode=0, flow_mode=2, work_mode=0;
        a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
        b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
        c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
        d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(conv_kernel<<12)|(register_mode<<10)|(flow_mode<<8)|(work_mode<<6)|flow_loop_times;
        f=(bias_address<<8)|id;
        __kuiloong_ace_msetins1(b,a);
        __kuiloong_ace_msetins2(d,c);
        __kuiloong_ace_msetins3(f,e);

        h_addr=(int32_t)(uintptr_t)(newA2 + us*new_K*i);
        v_addr=(int32_t)(uintptr_t)(newB2 + us*j);
        o_addr=(int32_t)(uintptr_t)(output_blockD + us*(new_N*i+j));
        conv_kernel=0, register_mode=0, flow_mode=0, work_mode=0;
        a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
        b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
        c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
        d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(conv_kernel<<12)|(register_mode<<10)|(flow_mode<<8)|(work_mode<<6)|flow_loop_times;
        e=1;
        f=(bias_address<<8)|id;
        __kuiloong_ace_msetins1(b,a);
        __kuiloong_ace_msetins2(d,c);
        __kuiloong_ace_msetins3(f,e);
      }
    }
    free(newA1);
    free(newA2);
    free(newB1);
    free(newB2);
  } else if ((K < 128) && (N == 128)) {
    ACENN_DEBUG("(K < 128) && (N == 128)");
    // 2. 判断是否因 o_x wrap而需要拆分
    int8_t  *newB1 = (int8_t*)malloc(K*N/2*sizeof(int8_t));
    int8_t  *newB2 = (int8_t*)malloc(K*N/2*sizeof(int8_t));
    int16_t *newD1 = (int16_t*)malloc(M*N/2*sizeof(int16_t));
    int16_t *newD2 = (int16_t*)malloc(M*N/2*sizeof(int16_t));
    _split_bigmat_128N(input_blockB, K, N, newB1, newB2);
    ACENN_DEBUG("newB1: %d, %d,%d,%d,%d,%d", newB1[0],newB1[1],newB1[2],newB1[3], newB1[4], newB1[5]);
    ACENN_DEBUG("newB2: %d, %d,%d,%d,%d,%d", newB2[0],newB2[1],newB2[2],newB2[3], newB2[4], newB2[5]);
    int new_M = M;
    int new_K = K;
    int new_N = N/2;
    int32_t h_ch=0,h_x=2*new_K/us,v_ch=us,v_x=new_N/us,o_ch=1,o_x=2*new_N/us;
    int32_t id=0,bias_address=0,flow_loop_times=new_K/us;
    int32_t stride_mode=0,transpose_mode=1;
    int32_t cutbit=5, shift_mode=1;
    for (int i = 0; i < new_M/us; i++) {
      for (int j = 0; j < new_N/us; j++) {
        int32_t h_addr=(int32_t)(uintptr_t)(input_blockA + us*new_K*i);
        int32_t v_addr=(int32_t)(uintptr_t)(newB1 + us*j);
        int32_t o_addr=(int32_t)(uintptr_t)(newD1 + us*(new_N*i + j));
        a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
        b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
        c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
        d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(0<<12)|(0<<10)|(0<<8)|(0<<6)|flow_loop_times;
        e=1;
        f=(bias_address<<8)|id;
        __kuiloong_ace_msetins1(b,a);
        __kuiloong_ace_msetins2(d,c);
        __kuiloong_ace_msetins3(f,e);

        h_addr=(int32_t)(uintptr_t)(input_blockA + us*new_K*i);
        v_addr=(int32_t)(uintptr_t)(newB2 + us*j);
        o_addr=(int32_t)(uintptr_t)(newD2 + us*(new_N*i + j));
        a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
        b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
        c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
        d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(0<<12)|(0<<10)|(0<<8)|(0<<6)|flow_loop_times;
        e=1;
        f=(bias_address<<8)|id;
        __kuiloong_ace_msetins1(b,a);
        __kuiloong_ace_msetins2(d,c);
        __kuiloong_ace_msetins3(f,e);
      }
    }

    // ACENN_DEBUG("============ output test ============");
    // for(int i = 0;i < M*N/2; ++i){if(newD1[i] != D_split_0_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, D_split_0_sa[i], newD1[i]);}}
    // for(int i = 0;i < M*N/2; ++i){if(newD2[i] != D_split_1_sa[i]){ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, D_split_1_sa[i], newD2[i]);}}
    // ACENN_DEBUG("============ output test ============");
    ACENN_DEBUG("============ Merge D ============");
    _merge_smallmat_128N(newD1, newD2, M, N, output_blockD);
    free(newB1);
    free(newB2);
    free(newD1);
    free(newD2);
  } else {
    int32_t h_ch=0,h_x=2*K/us,v_ch=us,v_x=N/us,o_ch=1,o_x=2*N/us;
    int32_t id=0,bias_address=0,flow_loop_times=K/us;
    int32_t stride_mode=0,transpose_mode=1;
    int32_t cutbit=5, shift_mode=1;
    for (int i = 0; i < M/us; i++) {
      for (int j = 0; j < N/us; j++) {
        int32_t h_addr=(int32_t)(uintptr_t)(input_blockA + us*K*i);
        int32_t v_addr=(int32_t)(uintptr_t)(input_blockB + us*j);
        int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
        a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
        b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
        c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
        d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(0<<12)|(0<<10)|(0<<8)|(0<<6)|flow_loop_times;
        e=1;
        f=(bias_address<<8)|id;
        __kuiloong_ace_msetins1(b,a);
        __kuiloong_ace_msetins2(d,c);
        __kuiloong_ace_msetins3(f,e);
      }
    }
  }
}