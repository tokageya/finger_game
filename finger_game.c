/*----------行動インデックスについて---------------*/
//それぞれの行動を以下のように設定する
/*
0. 行動無し(盤面が存在しないときの)
1. 自分の大きい指で相手の大きい指を攻撃する
2. 自分の大きい指で相手の小さい指を攻撃する
3. 自分の小さい指で相手の大きい指を攻撃する
4. 自分の小さい指で相手の小さい指を攻撃する
5. 自分の大きい指で自分の小さい指を攻撃する
6. 自分の小さい指で自分の大きい指を攻撃する
7. 自分の小さい指が1になるように攻撃する((自分の片方の指 0 && 自分のもう片方の指が 2 以上のとき) or (自分の指が両方とも 2 以上 && 自分の両方の指の合計が 5 以下のとき))
8. 自分の小さい指が2になるように攻撃する(??????Е???w??0, ???Е???w??4??????)
9. 自分の小さい指が3になるように攻撃する
*/
//※両手のそれぞれの指の値が同じ場合はそれぞれの対象を大きい指の値として行動インデックスを捉える
/*---------------------------------------------*/


/*==========================盤面ハッシュについて==========================*/

//それぞれの両手の情報は以下のように表せる
//(小さいほうの指の値, 大きいほうの指の値)というように記載

//  (0,0) (0,1) (0,2) (0,3) (0,4)
//    ０    １    ２    ３    ４
//        (1,1) (1,2) (1,3) (1,4)
//          ５    ６    ７    ８
//              (2,2) (2,3) (2,4)
//                ９   １０   １１
//                    (3,3) (3,4)
//                     １２  １３
//                          (4,4)
//                           １４

// それぞれの両手のindexは
//          small_num  = (小さいほうの指の値)
//          large_num  = (大きいほうの指の値)
// と変数を置くと,
//        (両手のindex値) =   small_num * (9 - small_num) / 2 + big
// と求めることができる


// index値は0～14と15進数で表せるため、
// 盤面ハッシュ   ( 自分の両手のindex値 ) × 15
//             + ( 相手の両手のindex値 )
//             = 盤面ハッシュ値
// とすることができる
/*==============================================================================*/


// 遺伝子情報について //
/*
「(盤面ハッシュ値)番目の場所に、ハッシュ値を複合化した盤面に対して最善だと考える行動インデックスを記した配列」
として表記し、プログラム内で使用する。
例：以下の盤面での行動について注目する
(相手側)                         (相手側)
  1 1    (自分の1で相手の1を攻撃)   1 2
                   ->
  1 1                              1 1
(味方側)                         (味方側)
このとき盤面{(1,1),(1,1)}に対応する盤面ハッシュは80なので、
個体情報配列の80番目に、行動「自分の小さい指で、相手の小さい指を攻撃する」に対応する行動インデックス値 1 を入れる。
*/

// 評価方法 //
/*
それぞれの盤面に対して、行動を適当に決めた相手と先攻、後攻の二回ゲームを行い、
勝ったら     +3 
負けたら     +0
引き分けは   +1
としてポイントを加算する
*/

// 選択方法 //
// とりあえずエリート選択

// 交叉方法 //
// とりあえず一様交叉

// 突然変異方法 //
//とりあえず個体突然変異率1%, 遺伝子突然変異率1%

#include<stdio.h>
#include<stdlib.h>
//#include<./make_csv.h>


#define GENOME_LENGTH 100 // 1世代の個体数
#define GENERATIONS_NUMBER 100 // 世代数
#define BOARD_NUM 225 // 盤面の総数

#define no_action 0 // 行動インデックス0. 行動なし

#define attack_b_to_b 1 // 行動インデックス1. 自分の大きい指で相手の大きい指を攻撃する
#define attack_b_to_s 2  // 行動インデックス2. 自分の大きい指で相手の小さい指を攻撃する

#define attack_s_to_b 3 // 行動インデックス3. 自分の小さい指で相手の大きい指を攻撃する
#define atttack_s_to_s 4 // 行動インデックス4. 自分の小さい指で相手の小さい指を攻撃する

#define attack_b_to_my_s 5 // 行動インデックス5. 自分の大きい指で自分の小さい指を攻撃する
#define attack_s_to_my_b 6 // 行動インデックス6. 自分の小さい指で自分の大きい指を攻撃する

#define devide_s1 7 // 行動インデックス7. 自分の小さい指が1になるように攻撃する
#define devide_s2 8 // 行動インデックス8. 自分の小さい指が2になるように攻撃する
#define devide_s3 9 // 行動インデックス9. 自分の小さい指が3になるように攻撃する

int main(void){
  unsigned int parents[GENOME_LENGTH][BOARD_NUM]; // (GENOME_LENGTH)個の親個体
  unsigned int children[GENOME_LENGTH][BOARD_NUM]; // (GENOME_LENGTH)個の子供個体
  int evaluations[GENOME_LENGTH]; // 現行世代に対応したそれぞれの評価値を登録
  unsigned int opponent[BOARD_NUM]; // 対戦相手のボード情報
  
  unsigned int action_arr[10]; // 盤面に対して打てる行動を入れた行動インデックス配列
  action_arr[0] = 1; // 自分の大きい指で相手の大きい指を攻撃する行動インデックス 1 を入れる
  unsigned int action_arr_len = 1; // 行動インデックス配列の長さ

  /* (GENOME_LENGTH)個の現行遺伝子個体をランダムに生成 */
  for(unsigned int gen = 0; gen < GENOME_LENGTH; gen ++){
    init_genome_random(children[gen]);
  }

  /*最初のランダムな遺伝子情報をcsvに出力*/
  output_genome_csv(children,"first_children.csv");

  /*世代数分loopする*/
  for(unsigned int generation = 1; generation <= GENERATIONS_NUMBER; generation++){
    /*childrenの値をparentsにコピー*/
    copy_genome(parents,children);

    /*それぞれの個体を評価して、個体を選択する*/
    evaluate_select_genome(parents,opponent,evaluations);

    /*親個体から交叉させて、子供個体を生成する*/
    generate_genome(parents,children);

    /*子供を突然変異させる*/
    mutate_genome(children);

    /*遺伝子情報を出力する*/
    printf("=========generation:%d==========\n",generation);
    print_genome(children);
    printf("================================\n");
  }

  /*最終世代を表示*/
  printf("-------------final generation--------------\n");
  print_genome(children);

  /*最終世代をcsvに出力*/
  output_genome_csv(children,"final_children.csv");

  return 0;
}





/*盤面ハッシュから自分の手の情報を返す*/
unsigned int my_hands_index(unsigned int board_hash){
  return (board_hash / 15);
}

/*盤面ハッシュから自分の手の情報を返す*/
unsigned int opponent_hands_index(unsigned int board_hash){
  return (board_hash % 15);
}

/*盤面ハッシュから自分の小さい指の値を返す*/
unsigned int my_small_finger(unsigned int board_hash){
  unsigned int index = my_hands_index(board_hash);
  if(index < 5) return 0;
  else if(index >= 5 && index <= 8) return 1;
  else if(index >= 9 && index <= 11) return 2;
  else if(index >= 12 && index <= 13) return 3;
  else return 4;
}

/*盤面ハッシュから自分の大きい指の値を返す*/
unsigned int my_big_finger(unsigned int board_hash){
  unsigned int index = my_hands_index(board_hash);
  if(index == 0) return 0;
  else if(index == 1 || index == 5) return 1;
  else if(index == 2 || index == 6 || index == 9) return 2;
  else if(index == 3 || index == 7 || index == 10 || index == 12) return 3;
  else return 4;
}

/*盤面ハッシュから相手の小さい指の値を返す*/
unsigned int opponent_small_finger(unsigned int board_hash){
  unsigned int index = opponent_hands_index(board_hash);
  if(index < 5) return 0;
  else if(index >= 5 && index <= 8) return 1;
  else if(index >= 9 && index <= 11) return 2;
  else if(index >= 12 && index <= 13) return 3;
  else return 4;
}

/*盤面ハッシュから相手の大きい指の値を返す*/
unsigned int opponent_big_finger(unsigned int board_hash){
  unsigned int index = opponent_hands_index(board_hash);
  if(index == 0) return 0;
  else if(index == 1 || index == 5) return 1;
  else if(index == 2 || index == 6 || index == 9) return 2;
  else if(index == 3 || index == 7 || index == 10 || index == 12) return 3;
  else return 4;
}



/*引数の配列の末尾に要素を追加する*/
void add_elem(unsigned int new_element, unsigned int* arr_len, unsigned int* arr){
  arr[(*arr_len)] = new_element;
  (*arr_len) ++;
  return;
}

/*行動の選択肢のリスト、行動インデックス配列を更新する(未完成)*/
void action_list(unsigned int board, unsigned int* arr_len , unsigned int* action_arr){
  unsigned int my_index = my_hands_index(board); // 自分の両手の情報
  unsigned int opponent_index = opponent_hands_index(board); // 相手の両手の情報

  unsigned int my_s = my_small_finger(board); // 自分の小さい指の情報
  unsigned int my_b = my_big_finger(board); // 自分の大きい指の情報
  unsigned int opp_s = opponent_small_finger(board); // 相手の小さい指の情報
  unsigned int opp_b = opponent_big_finger(board); // 相手の大きい指の情報
  
  (*arr_len) = 1;
  /*自分の両手の情報が(0,0) or 相手の両手の情報が(0,0) のとき、行動インデックス 0 を返す*/
  if(my_b == 0 || opp_b < 5){
    action_arr[0] = no_action;
    return;
  }

  /* 行動インデックス配列に行動インデックス 1 を入れる */
  action_arr[0] = attack_b_to_b;

  /* 相手の小さい指が 1 以上のとき
   * 自分の大きい指で相手の小さい指を攻撃する行動インデックス 2 を行動インデックス配列に入れる */
  if(my_s >= 1){
    action_arr[(*arr_len)] = attack_b_to_s;
    (*arr_len)++;
  }

  /* 自分の小さい指が 1 以上のとき
   * 自分の小さい指で相手の大きい指を攻撃する行動インデックス 3 を行動インデックス配列に入れる */
  if(my_s >= 1){
    action_arr[(*arr_len)] = attack_s_to_b;
    (*arr_len)++;
  }

  /* 自分の小さい指が 1 以上 && 相手の小さい指が 1 以上のとき
   * 自分の小さい指で相手の小さい指を攻撃する行動インデックス 4 を行動インデックス配列に入れる */
  if(my_s >= 1 && opp_s >= 1){
    action_arr[(*arr_len)] = atttack_s_to_s;
    (*arr_len)++;
  }

  /* ((自分の片方の指 0 && 自分のもう片方の指が 2 以上) or (自分の指が両方とも 2 以上 && 自分の両方の指の合計が 5 以下))のとき
   * 自分の小さい指が1になるように攻撃する行動インデックス 7 を行動インデックス配列に入れる */
  if( (my_index >= 2 && my_index <= 4) || (my_index) )
    add_elem(devide_s1,arr_len,action_arr);

  /**/
  
}

/*盤面に対し、ランダムな行動を返す(action_list配列ができれば完成)*/
unsigned int random_action_by_board(unsigned int board){
  unsigned int my_index = my_hands_index(board);//自分の両手の情報を入れる
  unsigned int opponent_index = opponent_hands_index(board);//相手の両手の情報を入れる


  /*盤面が存在しない(試合が終了している)場合は0を返す*/
  if(my_index < 5 || opponent_index < 5) return 0;
  
  /*ローカルの行動インデックス配列を作成する*/
  unsigned int action_arr[10];
  action_arr[0] = 1;
  unsigned int action_arr_len= 1;

  /*行動インデックス配列を更新する*/
  action_list(board, &action_arr_len, action_arr);

  /////*行動インデックス配列の中からランダムな値を返す*/////
  return action_arr[(rand()%(action_arr_len))];
}



/*一つの遺伝子にそれぞれの盤面に対しての行動をランダムに入れる(random_actioin_by_boardができれば完成)*/
void init_genome_random(unsigned int* genome){
  for(unsigned int board = 0; board < BOARD_NUM; board ++){
    genome[ board ] = random_action_by_board( board );
  }
  return;
}

/*次世代(子供)のそれぞれの遺伝子情報を現行世代(親)にコピーする(未完成)*/
void copy_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM],unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*現行世代(親)の情報をもとに敵の情報から評価する(未完成)*/
void evaluate_select_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM], unsigned int opponent[BOARD_NUM],int evaluations[GENOME_LENGTH]){
  return;
}

/*現行世代(親)の情報をもとに次世代(子)に交叉して遺伝させる(未完成)*/
void generate_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM],unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*遺伝子を一部突然変異させる(未完成)*/
void mutate_genome(unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*genome情報を出力する(未完成)*/
void print_genome(unsigned int genome[GENOME_LENGTH][BOARD_NUM]){
  return;
}

/*遺伝情報をcsvに出力する(未完成)*/
void output_genome_csv(unsigned int genome[GENOME_LENGTH][BOARD_NUM],char *fname){
  return;
}