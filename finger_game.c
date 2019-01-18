/*----------�s���C���f�b�N�X�ɂ���---------------*/
//���ꂼ��̍s�����ȉ��̂悤�ɐݒ肷��
/*
0. �s������(�Ֆʂ����݂��Ȃ��Ƃ���)

1. �����̑傫���w�ő���̑傫���w���U������
2. �����̑傫���w�ő���̏������w���U������

3. �����̏������w�ő���̑傫���w���U������
4. �����̏������w�ő���̏������w���U������

5. �����̑傫���w�Ŏ����̏������w���U������
6. �����̏������w�Ŏ����̑傫���w���U������

7. �����̏������w��1�ɂȂ�悤�ɍU������((�����̕Е��̎w 0 && �����̂����Е��̎w�� 2 �ȏ�̂Ƃ�) or (�����̎w�������Ƃ� 2 �ȏ� && �����̗����̎w�̍��v�� 5 �ȉ��̂Ƃ�))
8. �����̏������w��2�ɂȂ�悤�ɍU������(??????�E???w??0, ???�E???w??4??????)
9. �����̏������w��3�ɂȂ�悤�ɍU������
*/
//������̂��ꂼ��̎w�̒l�������ꍇ�͂��ꂼ��̑Ώۂ�傫���w�̒l�Ƃ��čs���C���f�b�N�X�𑨂���
/*---------------------------------------------*/


/*==========================�Ֆʃn�b�V���ɂ���==========================*/

//���ꂼ��̗���̏��͈ȉ��̂悤�ɕ\����
//(�������ق��̎w�̒l, �傫���ق��̎w�̒l)�Ƃ����悤�ɋL��

//  (0,0) (0,1) (0,2) (0,3) (0,4)
//    �O    �P    �Q    �R    �S
//        (1,1) (1,2) (1,3) (1,4)
//          �T    �U    �V    �W
//              (2,2) (2,3) (2,4)
//                �X   �P�O   �P�P
//                    (3,3) (3,4)
//                     �P�Q  �P�R
//                          (4,4)
//                           �P�S

// ���ꂼ��̗����index��
//          small_num  = (�������ق��̎w�̒l)
//          large_num  = (�傫���ق��̎w�̒l)
// �ƕϐ���u����,
//        (�����index�l) =   small_num * (9 - small_num) / 2 + big
// �Ƌ��߂邱�Ƃ��ł���


// index�l��0�`14��15�i���ŕ\���邽�߁A
// �Ֆʃn�b�V��   ( �����̗����index�l ) �~ 15
//             + ( ����̗����index�l )
//             = �Ֆʃn�b�V���l
// �Ƃ��邱�Ƃ��ł���
/*==============================================================================*/


// ��`�q���ɂ��� //
/*
�u(�Ֆʃn�b�V���l)�Ԗڂ̏ꏊ�ɁA�n�b�V���l�𕡍��������Ֆʂɑ΂��čőP���ƍl����s���C���f�b�N�X���L�����z��v
�Ƃ��ĕ\�L���A�v���O�������Ŏg�p����B

��F�ȉ��̔Ֆʂł̍s���ɂ��Ē��ڂ���
(���葤)                         (���葤)
  1 1    (������1�ő����1���U��)   1 2
                   ->
  1 1                              1 1
(������)                         (������)

���̂Ƃ��Ֆ�{(1,1),(1,1)}�ɑΉ�����Ֆʃn�b�V����80�Ȃ̂ŁA
�̏��z���80�ԖڂɁA�s���u�����̏������w�ŁA����̏������w���U������v�ɑΉ�����s���C���f�b�N�X�l 1 ������B

*/

// �]�����@ //
/*
���ꂼ��̔Ֆʂɑ΂��āA�s����K���Ɍ��߂�����Ɛ�U�A��U�̓��Q�[�����s���A
��������     +3 
��������     +0
����������   +1
�Ƃ��ă|�C���g�����Z����
*/

// �I����@ //
// �Ƃ肠�����G���[�g�I��

// �������@ //
// �Ƃ肠������l����

// �ˑR�ψٕ��@ //
//�Ƃ肠�����̓ˑR�ψٗ�1%, ��`�q�ˑR�ψٗ�1%

#include<stdio.h>
#include<stdlib.h>
//#include<./make_csv.h>


#define GENOME_LENGTH 100 // 1����̌̐�
#define GENERATIONS_NUMBER 100 // ���㐔
#define BOARD_NUM 225 // �Ֆʂ̑���

#define no_action 0 // �s���C���f�b�N�X0. �s���Ȃ�

#define attack_b_to_b 1 // �s���C���f�b�N�X1. �����̑傫���w�ő���̑傫���w���U������
#define attack_b_to_s 2  // �s���C���f�b�N�X2. �����̑傫���w�ő���̏������w���U������

#define attack_s_to_b 3 // �s���C���f�b�N�X3. �����̏������w�ő���̑傫���w���U������
#define atttack_s_to_s 4 // �s���C���f�b�N�X4. �����̏������w�ő���̏������w���U������

#define attack_b_to_my_s 5 // �s���C���f�b�N�X5. �����̑傫���w�Ŏ����̏������w���U������
#define attack_s_to_my_b 6 // �s���C���f�b�N�X6. �����̏������w�Ŏ����̑傫���w���U������

#define devide_s1 7 // �s���C���f�b�N�X7. �����̏������w��1�ɂȂ�悤�ɍU������
#define devide_s2 8 // �s���C���f�b�N�X8. �����̏������w��2�ɂȂ�悤�ɍU������
#define devide_s3 9 // �s���C���f�b�N�X9. �����̏������w��3�ɂȂ�悤�ɍU������

int main(void){
  unsigned int parents[GENOME_LENGTH][BOARD_NUM]; // (GENOME_LENGTH)�̐e��
  unsigned int children[GENOME_LENGTH][BOARD_NUM]; // (GENOME_LENGTH)�̎q����
  int evaluations[GENOME_LENGTH]; // ���s����ɑΉ��������ꂼ��̕]���l��o�^
  unsigned int opponent[BOARD_NUM]; // �ΐ푊��̃{�[�h���
  
  unsigned int action_arr[10]; // �Ֆʂɑ΂��đłĂ�s������ꂽ�s���C���f�b�N�X�z��
  action_arr[0] = 1; // �����̑傫���w�ő���̑傫���w���U������s���C���f�b�N�X 1 ������
  unsigned int action_arr_len = 1; // �s���C���f�b�N�X�z��̒���

  /* (GENOME_LENGTH)�̌��s��`�q�̂������_���ɐ��� */
  for(unsigned int gen = 0; gen < GENOME_LENGTH; gen ++){
    init_genome_random(children[gen]);
  }

  /*�ŏ��̃����_���Ȉ�`�q����csv�ɏo��*/
  output_genome_csv(children,"first_children.csv");

  /*���㐔��loop����*/
  for(unsigned int generation = 1; generation <= GENERATIONS_NUMBER; generation++){
    /*children�̒l��parents�ɃR�s�[*/
    copy_genome(parents,children);

    /*���ꂼ��̌̂�]�����āA�̂�I������*/
    evaluate_select_genome(parents,opponent,evaluations);

    /*�e�̂�����������āA�q���̂𐶐�����*/
    generate_genome(parents,children);

    /*�q����ˑR�ψق�����*/
    mutate_genome(children);

    /*��`�q�����o�͂���*/
    printf("=========generation:%d==========\n",generation);
    print_genome(children);
    printf("================================\n");
  }

  /*�ŏI�����\��*/
  printf("-------------final generation--------------\n");
  print_genome(children);

  /*�ŏI�����csv�ɏo��*/
  output_genome_csv(children,"final_children.csv");

  return 0;
}





/*�Ֆʃn�b�V�����玩���̎�̏���Ԃ�*/
unsigned int my_hands_index(unsigned int board_hash){
  return (board_hash / 15);
}

/*�Ֆʃn�b�V�����玩���̎�̏���Ԃ�*/
unsigned int opponent_hands_index(unsigned int board_hash){
  return (board_hash % 15);
}

/*�Ֆʃn�b�V�����玩���̏������w�̒l��Ԃ�*/
unsigned int my_small_finger(unsigned int board_hash){
  unsigned int index = my_hands_index(board_hash);
  if(index < 5) return 0;
  else if(index >= 5 && index <= 8) return 1;
  else if(index >= 9 && index <= 11) return 2;
  else if(index >= 12 && index <= 13) return 3;
  else return 4;
}

/*�Ֆʃn�b�V�����玩���̑傫���w�̒l��Ԃ�*/
unsigned int my_big_finger(unsigned int board_hash){
  unsigned int index = my_hands_index(board_hash);
  if(index == 0) return 0;
  else if(index == 1 || index == 5) return 1;
  else if(index == 2 || index == 6 || index == 9) return 2;
  else if(index == 3 || index == 7 || index == 10 || index == 12) return 3;
  else return 4;
}

/*�Ֆʃn�b�V�����瑊��̏������w�̒l��Ԃ�*/
unsigned int opponent_small_finger(unsigned int board_hash){
  unsigned int index = opponent_hands_index(board_hash);
  if(index < 5) return 0;
  else if(index >= 5 && index <= 8) return 1;
  else if(index >= 9 && index <= 11) return 2;
  else if(index >= 12 && index <= 13) return 3;
  else return 4;
}

/*�Ֆʃn�b�V�����瑊��̑傫���w�̒l��Ԃ�*/
unsigned int opponent_big_finger(unsigned int board_hash){
  unsigned int index = opponent_hands_index(board_hash);
  if(index == 0) return 0;
  else if(index == 1 || index == 5) return 1;
  else if(index == 2 || index == 6 || index == 9) return 2;
  else if(index == 3 || index == 7 || index == 10 || index == 12) return 3;
  else return 4;
}



/*�����̔z��̖����ɗv�f��ǉ�����*/
void add_elem(unsigned int new_element, unsigned int* arr_len, unsigned int* arr){
  arr[(*arr_len)] = new_element;
  (*arr_len) ++;
  return;
}

/*�s���̑I�����̃��X�g�A�s���C���f�b�N�X�z����X�V����(������)*/
void action_list(unsigned int board, unsigned int* arr_len , unsigned int* action_arr){
  unsigned int my_index = my_hands_index(board); // �����̗���̏��
  unsigned int opponent_index = opponent_hands_index(board); // ����̗���̏��

  unsigned int my_s = my_small_finger(board); // �����̏������w�̏��
  unsigned int my_b = my_big_finger(board); // �����̑傫���w�̏��
  unsigned int opp_s = opponent_small_finger(board); // ����̏������w�̏��
  unsigned int opp_b = opponent_big_finger(board); // ����̑傫���w�̏��
  
  (*arr_len) = 1;
  /*�����̗���̏��(0,0) or ����̗���̏��(0,0) �̂Ƃ��A�s���C���f�b�N�X 0 ��Ԃ�*/
  if(my_b == 0 || opp_b < 5){
    action_arr[0] = no_action;
    return;
  }

  /* �s���C���f�b�N�X�z��ɍs���C���f�b�N�X 1 ������ */
  action_arr[0] = attack_b_to_b;

  /* ����̏������w�� 1 �ȏ�̂Ƃ�
   * �����̑傫���w�ő���̏������w���U������s���C���f�b�N�X 2 ���s���C���f�b�N�X�z��ɓ���� */
  if(my_s >= 1){
    action_arr[(*arr_len)] = attack_b_to_s;
    (*arr_len)++;
  }

  /* �����̏������w�� 1 �ȏ�̂Ƃ�
   * �����̏������w�ő���̑傫���w���U������s���C���f�b�N�X 3 ���s���C���f�b�N�X�z��ɓ���� */
  if(my_s >= 1){
    action_arr[(*arr_len)] = attack_s_to_b;
    (*arr_len)++;
  }

  /* �����̏������w�� 1 �ȏ� && ����̏������w�� 1 �ȏ�̂Ƃ�
   * �����̏������w�ő���̏������w���U������s���C���f�b�N�X 4 ���s���C���f�b�N�X�z��ɓ���� */
  if(my_s >= 1 && opp_s >= 1){
    action_arr[(*arr_len)] = atttack_s_to_s;
    (*arr_len)++;
  }

  /* ((�����̕Е��̎w 0 && �����̂����Е��̎w�� 2 �ȏ�) or (�����̎w�������Ƃ� 2 �ȏ� && �����̗����̎w�̍��v�� 5 �ȉ�))�̂Ƃ�
   * �����̏������w��1�ɂȂ�悤�ɍU������s���C���f�b�N�X 7 ���s���C���f�b�N�X�z��ɓ���� */
  if( (my_index >= 2 && my_index <= 4) || (my_index) )
    add_elem(devide_s1,arr_len,action_arr);

  /**/
  
}

/*�Ֆʂɑ΂��A�����_���ȍs����Ԃ�(action_list�z�񂪂ł���Ί���)*/
unsigned int random_action_by_board(unsigned int board){
  unsigned int my_index = my_hands_index(board);//�����̗���̏�������
  unsigned int opponent_index = opponent_hands_index(board);//����̗���̏�������


  /*�Ֆʂ����݂��Ȃ�(�������I�����Ă���)�ꍇ��0��Ԃ�*/
  if(my_index < 5 || opponent_index < 5) return 0;
  
  /*���[�J���̍s���C���f�b�N�X�z����쐬����*/
  unsigned int action_arr[10];
  action_arr[0] = 1;
  unsigned int action_arr_len= 1;

  /*�s���C���f�b�N�X�z����X�V����*/
  action_list(board, &action_arr_len, action_arr);

  /////*�s���C���f�b�N�X�z��̒����烉���_���Ȓl��Ԃ�*/////
  return action_arr[(rand()%(action_arr_len))];
}



/*��̈�`�q�ɂ��ꂼ��̔Ֆʂɑ΂��Ă̍s���������_���ɓ����(random_actioin_by_board���ł���Ί���)*/
void init_genome_random(unsigned int* genome){
  for(unsigned int board = 0; board < BOARD_NUM; board ++){
    genome[ board ] = random_action_by_board( board );
  }
  return;
}

/*������(�q��)�̂��ꂼ��̈�`�q�������s����(�e)�ɃR�s�[����(������)*/
void copy_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM],unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*���s����(�e)�̏������ƂɓG�̏�񂩂�]������(������)*/
void evaluate_select_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM], unsigned int opponent[BOARD_NUM],int evaluations[GENOME_LENGTH]){
  return;
}

/*���s����(�e)�̏������ƂɎ�����(�q)�Ɍ������Ĉ�`������(������)*/
void generate_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM],unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*��`�q���ꕔ�ˑR�ψق�����(������)*/
void mutate_genome(unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*genome�����o�͂���(������)*/
void print_genome(unsigned int genome[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*��`����csv�ɏo�͂���(������)*/
void output_genome_csv(unsigned int genome[GENOME_LENGTH][BOARD_NUM],char *fname){
  return;
}
