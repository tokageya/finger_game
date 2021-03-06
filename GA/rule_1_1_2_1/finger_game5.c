////////途中////////






/*----------行動インデックスについて---------------*/
/* それぞれの行動とそれに対応する行動インデックスを以下のように設定する
 * 0. 行動無し(盤面が存在しないときの)
 * 1. 自分の大きい指で相手の大きい指を攻撃する
 * 2. 自分の大きい指で相手の小さい指を攻撃する
 * 3. 自分の小さい指で相手の大きい指を攻撃する
 * 4. 自分の小さい指で相手の小さい指を攻撃する
 * 5. 自分の大きい指で自分の小さい指を攻撃する
 * 6. 自分の小さい指で自分の大きい指を攻撃する
 * 7. 自分の小さい指が1になるように分割する
 * 8. 自分の小さい指が2になるように分割する
 * 9. 自分の小さい指が3になるように分割する
 * //※両手のそれぞれの指の値が同じ場合はそれぞれの対象を小さい指の値として行動インデックスを捉える
*/
/*---------------------------------------------*/


/*==========================盤面ハッシュについて==========================*/
/* それぞれの盤面とそれに対応する盤面ハッシュを以下のように設定する
 * //それぞれの両手の情報は以下のように表せる
 * //(小さいほうの指の値, 大きいほうの指の値)というように記載

 *   (0,0) (0,1) (0,2) (0,3) (0,4)
 *     ０    １    ２    ３    ４
 *         (1,1) (1,2) (1,3) (1,4)
 *           ５    ６    ７    ８
 *              (2,2) (2,3) (2,4)
 *                ９   １０   １１
 *                    (3,3) (3,4)
 *                     １２  １３
 *                          (4,4)
 *                           １４

 * それぞれの両手のindexは
 *          small_num  = (小さいほうの指の値)
 *          large_num  = (大きいほうの指の値)
 * と変数を置くと,
 *        (両手のindex値) =   small_num * (9 - small_num) / 2 + big
 * と求めることができる


 * index値は0～14と15進数で表せるため、
 * 盤面ハッシュ   ( 自分の両手のindex値 ) × 15
 *             + ( 相手の両手のindex値 )
 *             = 盤面ハッシュ値
 * とすることができる
*/
/*==============================================================================*/


// 符号化方法(解の表現方法)について //
/* 符号化方法は、
 * 「(盤面ハッシュ値)番目の場所に、ハッシュ値を複合化した盤面に対して最善だと考える行動インデックスを記した配列」
 * として表記し、プログラム内で使用する。
 * 例：以下の盤面での行動について注目する
 * (相手側)                         (相手側)
 *   1 1    (自分の1で相手の1を攻撃)   1 2
                   ->
 *   1 1                              1 1
 * (味方側)                         (味方側)
 * このとき盤面{(1,1),(1,1)}に対応する盤面ハッシュは80なので、
 * 個体情報配列の80番目に、行動「自分の小さい指で、相手の小さい指を攻撃する」に対応する行動インデックス値 1 を入れる。
*/

// 評価方法 //
/*評価方法は、
 * それぞれの個体が総当たり戦をして、
 * 勝ったら     +3 
 * 負けたら     +0
 * 引き分けは   +1 
 * としてポイントを加算する
*/

// 選択方法 //
/* お互いを戦わせてその結果からエリート選択
 * */
// 交叉方法 //
// 一様交叉

// 突然変異方法 //
//個体突然変異率5%, 遺伝子突然変異率5%

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
//#include<./make_csv.h>


#define GENOME_LENGTH 100 // 1世代の個体数
#define GENERATIONS_NUMBER 500 // 世代数
#define BOARD_NUM 225 // 盤面の総数

#define no_action 0 // 行動インデックス0. 行動なし
#define attack_b_to_b 1 // 行動インデックス1. 自分の大きい指で相手の大きい指を攻撃する
#define attack_b_to_s 2  // 行動インデックス2. 自分の大きい指で相手の小さい指を攻撃する

#define attack_s_to_b 3 // 行動インデックス3. 自分の小さい指で相手の大きい指を攻撃する
#define atttack_s_to_s 4 // 行動インデックス4. 自分の小さい指で相手の小さい指を攻撃する

#define attack_b_to_my_s 5 // 行動インデックス5. 自分の大きい指で自分の小さい指を攻撃する
#define attack_s_to_my_b 6 // 行動インデックス6. 自分の小さい指で自分の大きい指を攻撃する

#define devide_s1 7 // 行動インデックス7. 自分の小さい指が1になるように分割する
#define devide_s2 8 // 行動インデックス8. 自分の小さい指が2になるように分割する
#define devide_s3 9 // 行動インデックス9. 自分の小さい指が3になるように分割する

#define ACTION_CHOICES 10 // 行動インデックスの総数
#define MAX_LOG 200 // ボードの履歴を格納する数
#define ELITE_GENOME_NUM 10 // 次世代に残す現行のエリート遺伝子の数
#define ELITE_PROGENCY_GENOME_NUM 60 // 交叉させて生み出す次世代の遺伝子の数
#define OTHER_GENOME_NUM 30 // 次世代に残すその他の遺伝子

/*マイクロ秒で乱数を返す*/
unsigned int my_rand(void){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec);
  return tv.tv_usec*rand();
}

/*勝敗判定で使う judge 構造体を作成*/
typedef struct Judge_{
  unsigned int goalpoints_difference;
  unsigned int play_first;
  unsigned int draw_first;
} judge;

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

/*自分と相手それぞれの手の情報から盤面ハッシュを返す(復号化)*/
unsigned int decoding_board_hash(unsigned int player1_r,unsigned int player1_l, unsigned int player2_r, unsigned int player2_l){
  unsigned int player1_s,player1_b,player2_s,player2_b;
  if(player1_r < player1_l){ player1_s = player1_r; player1_b = player1_l;}
  else{ player1_s = player1_l; player1_b = player1_r;}
  if(player2_r < player2_l){ player2_s = player2_r; player2_b = player2_l;}
  else{ player2_s = player2_l; player2_b = player2_r;}
  return ( ( 15 * ( ((player1_s * ( 9 - player1_s )) / 2)  + player1_b) ) + ( ((player2_s * ( 9 - player2_s )) / 2)  + player2_b) );
}

/*引数の配列の末尾に要素を追加する*/
void add_elem(unsigned int new_element, unsigned int* arr_len, unsigned int* arr){
  arr[(*arr_len)] = new_element;
  (*arr_len) ++;
  return;
}

/*行動の選択肢のリスト、行動インデックス配列を更新する*/
void action_list(unsigned int board, unsigned int* arr_len , unsigned int* action_arr){
  //unsigned int my_index = my_hands_index(board); // 自分の両手の情報
  //unsigned int opponent_index = opponent_hands_index(board); // 相手の両手の情報

  unsigned int my_s = my_small_finger(board); // 自分の小さい指の情報
  unsigned int my_b = my_big_finger(board); // 自分の大きい指の情報
  unsigned int opp_s = opponent_small_finger(board); // 相手の小さい指の情報
  unsigned int opp_b = opponent_big_finger(board); // 相手の大きい指の情報
  
  (*arr_len) = 0; // (*arr_len) を初期化

  /*自分の両手の情報が(0,0) or 相手の両手の情報が(0,0) のとき、行動インデックス 0 を返す*/
  if(my_b == 0 || opp_b == 0){
    add_elem(no_action,arr_len,action_arr);
    return;
  }

  /* 行動インデックス配列に行動インデックス 1 を入れる */
  add_elem(attack_b_to_b,arr_len,action_arr);

  /* ( 相手の小さい指が 1 以上 && 相手の両手の指が一緒じゃない ) のとき
   * 自分の大きい指で相手の小さい指を攻撃する行動インデックス 2 を行動インデックス配列に入れる */
  if( ( opp_s >= 1 ) && (opp_s != opp_b) ){
    add_elem(attack_b_to_s,arr_len,action_arr);
  }

  /* ( 自分の小さい指が 1 以上 && 自分の両手の指が一緒じゃない ) のとき
   * 自分の小さい指で相手の大きい指を攻撃する行動インデックス 3 を行動インデックス配列に入れる */
  if( ( my_s >= 1 ) && ( my_s != my_b ) ){
    add_elem(attack_s_to_b,arr_len,action_arr);
  }

  /* ( ( 自分の小さい指が 1 以上 && 自分の両手の指が一緒じゃない ) && ( 相手の小さい指が 1 以上 && 相手の両手の指が一緒じゃない ) ) のとき
   * 自分の小さい指で相手の小さい指を攻撃する行動インデックス 4 を行動インデックス配列に入れる */
  if( ( (my_s >= 1) && (my_s != my_b) ) && ( (opp_s >= 1) && (opp_s != opp_b) ) ){
    add_elem(atttack_s_to_s,arr_len,action_arr);
  }

  /* 自分の小さい指が 1 以上のとき
   * 自分の大きい指で自分の小さい指を攻撃する行動インデックス 5 を入れ、
   * ( 自分の小さい指が 1 以上 && 自分の両手の指が一緒じゃない ) とき
   * 自分の大きい指で自分の小さい指を攻撃する行動インデックス 6 の二つを　行動インデックス配列に入れる */
  if(my_s >= 1){
    add_elem(attack_b_to_my_s,arr_len,action_arr);
    if(my_s != my_b)add_elem(attack_s_to_my_b,arr_len,action_arr);
  }

  /* ( (自分の小さい指が 0 && 自分の大きい指が 2 以上) or (自分の指が両方とも 2 以上 && 自分の両方の指の合計が 5 以下) ) のとき
   * 自分の小さい指が1になるように分割する行動インデックス 7 を行動インデックス配列に入れる */
  /* 盤面の移動例
   * (0,2)->(1,1)
   * (0,3)->(1,2)
   * (0,4)->(1,3)
   * (2,2)->(1,3)
   * (2,3)->(1,4)
  */
  if( (my_s == 0 && my_b >= 2) || (my_s >= 2 && ((my_s + my_b) <= 5) ) ){
    add_elem(devide_s1,arr_len,action_arr);
  }

  /* ( (自分の小さい指が 0 && 自分の大きい指が 4) or (自分の小さい指が 1 && 自分の大きい指が 3 以上) or (自分の小さい指が 3 && 自分の大きい指が 3) ) のとき
   * 自分の小さい指が2になるように分割する行動インデックス 8 を行動インデックス配列に入れる */
  /* 盤面の移動例
   * (0,4)->(2,2)
   * (1,3)->(2,2)
   * (1,4)->(2,3)
   * (3,3)->(2,4)
  */
  if((my_s == 0 && my_b == 4) || ( (my_s == 1 && my_b >= 3) || (my_s == 3 && my_b == 3) ) ){
    add_elem(devide_s2,arr_len,action_arr);
  }

  /* 自分の小さい指が
   * 自分の小さい指が3になるように分割する行動インデックス 9 を行動インデックス配列に入れる */
  /* 盤面の移動例
   * (2,4)->(3,3)
  */
  if(my_s == 2 && my_b == 4){
    add_elem(devide_s3,arr_len,action_arr);
  }

  return;  
}

/*盤面に対し、ランダムな行動を返す*/
unsigned int random_action_by_board(unsigned int board){
  unsigned int my_index = my_hands_index(board);//自分の両手の情報を入れる
  unsigned int opponent_index = opponent_hands_index(board);//相手の両手の情報を入れる


  /*盤面が存在しない(試合が終了している)場合は0を返す*/
  if(my_index == 0 || opponent_index == 0) return 0;
  
  /*ローカルの行動インデックス配列を作成する*/
  unsigned int action_arr[ACTION_CHOICES];
  action_arr[0] = 1;
  unsigned int action_arr_len= 1;

  /*行動インデックス配列を更新する*/
  //printf("action_list start.\n");
  action_list(board, &action_arr_len, action_arr);

  /////*行動インデックス配列の中からランダムな値を返す*/////
  return action_arr[ ( my_rand() % (action_arr_len) ) ];
}

/*一つの遺伝子にそれぞれの盤面に対しての行動をランダムに入れる*/
void init_genome_random(unsigned int genome[BOARD_NUM]){
  for(unsigned int board = 0; board < BOARD_NUM; board ++){
    genome[ board ] = random_action_by_board( board );
  }
  return;
}

/*次世代(子供)のそれぞれの遺伝子情報を現行世代(親)にコピーする*/
void copy_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM],unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  for(unsigned int gen = 0; gen < GENOME_LENGTH; gen++){
    for(unsigned int board = 0; board < BOARD_NUM; board++){
      parents[ gen ][ board ] = children[ gen ][ board ];
    }
  }
  return;
}

/*盤面ハッシュを一時的に相手視点に変えるための関数*/
unsigned int reverse_board(unsigned int board){
  unsigned int board2 = (opponent_hands_index(board) * 15) + my_hands_index(board);
  return board2;
}

/*引数の盤面ハッシュに対応する盤面から、引数の行動インデックスに対応する行動をとると遷移する次の盤面を返す*/

//同じ盤面でも相手か自分かで区別して

unsigned int next_board(unsigned int now_board, unsigned int action){
  unsigned int player1_s = my_small_finger(now_board);
  unsigned int player1_b = my_big_finger(now_board);
  unsigned int player2_s = opponent_small_finger(now_board);
  unsigned int player2_b = opponent_big_finger(now_board);
  //unsigned int board = now_board;
  /*行動インデックス毎に場合分け*/
  if(action == attack_b_to_b/*1*/){
    player2_b += player1_b;
  }
  else if(action == attack_b_to_s/*2*/){
    if(player2_s == 0) player2_b += player1_b;
    else player2_s += player1_b;
  }
  else if(action == attack_s_to_b/*3*/){
    if(player1_s == 0) player2_b += player1_b;
    else player2_b += player1_s;
  }
  else if(action == atttack_s_to_s/*4*/){
    if(player1_s == 0 && player2_s == 0) player2_b += player1_b;
    else if(player1_s == 0) player2_s += player1_b;
    else if(player2_s == 0) player2_b += player1_s;
    else player2_s += player1_s;
  }
  else if(action == attack_b_to_my_s/*5*/){
    player1_s += player1_b;
  }
  else if(action == attack_s_to_my_b/*6*/){
    player1_b += player1_s;
  }
  else if(action == devide_s1/*7*/){
    unsigned int sum = player1_b + player1_s;
    player1_s = 1;
    player1_b = sum-1;
  }
  else if(action == devide_s2/*8*/){
    unsigned int sum = player1_b + player1_s;
    player1_s = 2;
    player1_b = sum - 2;
    
  }
  else if(action == devide_s3/*9*/){
    unsigned int sum = player1_b + player1_s;
    player1_s = 3;
    player1_b = sum-3;
  }
  if(player1_b >= 5) player1_b = 0;
  //player1_b = player1_b % 5;
  if(player1_s >= 5) player1_s = 0;
  //player1_s = player1_s % 5;
  if(player2_b >= 5) player2_b = 0;
  //player2_b = player2_b % 5;
  if(player2_s >= 5) player2_s = 0;
  //player2_s = player2_s % 5;

  return decoding_board_hash(player1_s,player1_b,player2_s,player2_b);
}

/*引数の盤面のログからループしていないかどうかを確認する*/
/*ループしていたら 1 を返す*/
unsigned int loop_check(unsigned int board_log_len, unsigned int* board_log){
  for(unsigned int turn = 0; turn < board_log_len; turn ++){
    unsigned int a_board = board_log[turn];
    for(unsigned int i = turn+1; i < board_log_len; i++){
      if(a_board == board_log[i]) return 1;
    }
  }
  return 0;
}

/*引数で得た二つの個体を先攻,後攻の両方で戦わせて、それぞれの結果からplayer1の評価を judge 変数に格納して返す*/
judge battle(unsigned int player1[BOARD_NUM], unsigned int player2[BOARD_NUM]){
  judge j = {0,0,0};

  unsigned int board = 80; // (先攻側から見た)盤面の状況のハッシュ値を格納
  unsigned int turn_num = 0; // ターン数を格納
  unsigned int board_log[MAX_LOG]; // ボードの履歴を格納。「配列のn番目の要素 = nターン目の盤面ハッシュ」とする。
  board_log[0] = 80;//0ターン目を最初の配列の先頭とする
  unsigned int board_log_len = 1;
  unsigned int loop_flag = 0; // 盤面がループしていた時に立てるフラグ
  
  /*==================== player1を先攻として戦う ====================*/
  while(my_big_finger(board) != 0 && opponent_big_finger(board) != 0 && !(loop_flag)){
    /*ターンを進める*/
    turn_num++;
    if(turn_num % 2 == 1) board = next_board(board, player1[board]);
    else{
      unsigned int board2 = reverse_board(board);//相手視点の盤面ハッシュ
      board = reverse_board( next_board( board2, player2[board2]) );
    }
    add_elem(board,&board_log_len,board_log);
    /*ループしていないか確認する*/
    loop_flag = loop_check(board_log_len,board_log);
  }
  /*結果をjudge変数に格納*/
  if(loop_flag){
    j.play_first += 1;
  }
  else if(opponent_big_finger(board) == 0){
    j.play_first += 3;
  }
  else{
    j.play_first = 0;
  }
  j.goalpoints_difference += j.play_first;

  board = 80; // 盤面を初期化
  turn_num = 0; // ターン数を初期化
  //board_log[MAX_LOG] = ; // ボードの履歴を初期化
  board_log[0] = 80;//0ターン目を最初の配列の先頭とする
  board_log_len = 1;
  loop_flag = 0; // 盤面がループしていた時に立てるフラグの初期化

  /*==================== player1を後攻として戦う ====================*/
  while(my_big_finger(board) != 0 && opponent_big_finger(board) != 0 && !(loop_flag)){
    /*ターンを進める*/
    turn_num++;
    if(turn_num % 2 == 0) board = next_board(board, player1[board]);
    else{
      unsigned int board2 = reverse_board(board);//相手視点の盤面ハッシュ
      board = reverse_board( next_board( board2, player2[board2]) );
    }
    add_elem(board,&board_log_len,board_log);
    /*ループしていないか確認する*/
    loop_flag = loop_check(board_log_len,board_log);
  }
  /*結果をjudge変数に格納*/
  if(loop_flag){
    j.draw_first += 1;
  }
  else if(opponent_big_finger(board) == 0){
    j.draw_first += 3;
  }
  else{
    j.draw_first = 0;
  }
  j.goalpoints_difference += j.draw_first;


  return j;
}

/*現行世代(親)の情報をもとに敵の情報から評価する*/ //現行世代どおしを戦わせて評価するトーナメント方式
void evaluate_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM], unsigned int opponent[BOARD_NUM],judge evaluations[GENOME_LENGTH]){
  /*evaluations関数を初期化*/
  for(unsigned int gen = 0; gen < GENOME_LENGTH; gen++){
    evaluations[ gen ].goalpoints_difference = 0;
    evaluations[ gen ].play_first = 0;
    evaluations[ gen ].draw_first = 0;
  }

  /*それぞれの個体を戦わせて、評価を evalutions 配列の添え字が対応する場所に入れる*/
  for(unsigned int gen1 = 0; gen1 < GENOME_LENGTH; gen1++){
    for(unsigned int gen2 = gen1 + 1; gen2 < GENOME_LENGTH; gen2++){
      judge j1 = battle( parents[ gen1 ] , parents[ gen2 ] );
      judge j2 = battle( parents[ gen2 ] , parents[ gen1 ] );

      evaluations[ gen1 ].goalpoints_difference += j1.goalpoints_difference;
      evaluations[ gen1 ].play_first += j1.play_first;
      evaluations[ gen1 ].draw_first += j1.draw_first;
      evaluations[ gen2 ].goalpoints_difference += j2.goalpoints_difference;
      evaluations[ gen2 ].play_first += j2.play_first;
      evaluations[ gen2 ].draw_first += j2.draw_first;
    }
    
    //evaluations[ gen1 ] = battle( parents[ gen1 ] , opponent);
  }
  return;
}

/*引数の配列について、添え字の要素を削除して配列を更新する*/ //使っていない
void delete_arr_elem(unsigned int position, unsigned int *arr_len, unsigned int *arr){
  for(int i = 0; i < position; i ++)arr++;
  for(int i = position; i < ((*arr_len)-1); i++)arr[i] = arr[i + 1];
  (*arr_len)--;
  return;
}

/*引数の2つの遺伝子について、配列のある2つポジションで交叉させて更新する*/
void cross_over(unsigned int parent_1[BOARD_NUM], unsigned int parent_2[BOARD_NUM], unsigned int child[BOARD_NUM] ){
  for(unsigned int board = 0; board < BOARD_NUM; board++){
    if(my_rand()%2 == 1) child[board] = parent_1[board];    
    else child[board] = parent_2[board];
  }
}

/*現行世代(親)の情報をもとに次世代(子)に交叉して遺伝させる*/
void generate_genome(unsigned int parents[GENOME_LENGTH][BOARD_NUM],unsigned int children[GENOME_LENGTH][BOARD_NUM], judge evaluations[GENOME_LENGTH]){
  /*現行世代で良い結果を持っている親を捜す*/
  unsigned int elite_arr[GENOME_LENGTH][BOARD_NUM] = {};
  unsigned int elite_arr_len = 0;
  unsigned int elite_situ = 0; // 最終的に親として選んだ個体の中で最も低い評価の値が入る
  unsigned int elite_position = 0;
  while(elite_arr_len < ELITE_GENOME_NUM){
    elite_situ = 0;//リセット
    elite_position = 0;//リセット
    /*まずは現行世代の中でどれが一番良い個体であるかを捜す*/
    for(unsigned int gen = 0; gen < GENOME_LENGTH; gen++){
      if(elite_situ < evaluations[gen].goalpoints_difference){
        
        elite_situ = evaluations[gen].goalpoints_difference;
        elite_position = gen;
      }
    }
    //printf("===============================================\n======================================\n===============================");
    //printf("now_elite_situ:%d | ",elite_situ);
    /*現行世代の中で良い個体をエリート個体配列に入れる*/
    
    for(unsigned int board = 0; board < BOARD_NUM; board++){
      elite_arr[elite_arr_len][board] = parents[elite_position][board];
      //printf("elite_arr[%d] = %d | \n",elite_arr_len,elite_arr[elite_arr_len][board]);
    
      //children[elite_arr_len][board] = parents[elite_position][board];
    }
    
    
    evaluations[elite_position].goalpoints_difference = 0;//選び終わった個体の評価を0に戻す
    elite_arr_len++;
    //printf("\nelite_arr_len = %d\n",elite_arr_len);
    

    //printf("now_most_evaluations:%d | ",elite_situ);
  }
  //printf("now_most_elite_evaluations:%d | ",elite_situ);



  /*エリート個体のうち、(ELITE_GENOME_NUM)体を次世代に入れる*/
  for(int eli_gen = 0; eli_gen < ELITE_GENOME_NUM; eli_gen++){
    for(int board = 0; board < BOARD_NUM; board++){
      //printf("elite_arr[%d][%d]=%d|",eli_gen,board,elite_arr[eli_gen][board]);
      children[eli_gen][board] = elite_arr[eli_gen][board];
    }
  }
  

  /*エリート個体を一様交叉させ、(ELITE_PROGENCY_NUM)体を次世代に残す*/
  unsigned int gen1, gen2;
  //unsigned int cross_1, cross_2;
  for(unsigned int n_gen = ELITE_GENOME_NUM; n_gen < (ELITE_GENOME_NUM + ELITE_PROGENCY_GENOME_NUM); n_gen++){
    /*エリート個体からランダムに二つ選択する*/
    gen1 = my_rand() % ELITE_GENOME_NUM;
    gen2 = my_rand() % ELITE_GENOME_NUM;
    while(gen1 == gen2){
      gen1 = my_rand() % ELITE_GENOME_NUM;
      gen2 = my_rand() % ELITE_GENOME_NUM;
    }
    cross_over(elite_arr[gen1], elite_arr[gen2], children[ n_gen ]);
  }
  
  /*エリート以外の現行遺伝子をランダムに入れる*/
  for(unsigned int now_gen = ( ELITE_PROGENCY_GENOME_NUM + ELITE_GENOME_NUM ); now_gen < GENOME_LENGTH; now_gen ++){
    if(evaluations[now_gen].goalpoints_difference <= elite_situ){
      for(int board = 0; board < BOARD_NUM; board++){
        children[now_gen][board] = parents[now_gen][board];
      }
    }
  }
  
  return;
}

/*配列の中に要素が入っているかどうか判定する*/
unsigned int include_arr(unsigned int el,unsigned int arr_len, unsigned int arr[ACTION_CHOICES]){
  for(int i = 0; i < arr_len; i++){
    if(arr[i] == el)return 1;
  }
  return 0;
}

/*5つの盤面ハッシュを受け取り、それがすべて違うのであれば0を返し、同じものが含まれていれば換えるべきボードの数を返す*/
unsigned int match_board(unsigned int board1,unsigned int board2,unsigned int board3,unsigned int board4,unsigned int board5){
  if(board1 == board2 || board1 == board3 || board1 == board4 || board1 == board5) return 1;
  else if(board2 == board3 || board2 == board4 || board2 == board5) return 2;
  else if(board3 == board4 || board3 == board5) return 3;
  else if(board4 == board5) return 4;
  else return 0;
}
/*5つの盤面ハッシュを受け取り、それが全てプレイ中に出る可能性のある盤面であれば0を返し、存在しない盤面が含まれている場合はそのボードの数を返す*/
unsigned int board_exist(unsigned int board1,unsigned int board2,unsigned int board3,unsigned int board4,unsigned int board5){
  if(my_big_finger(board1) == 0 || opponent_big_finger(board1) == 0) return 1;
  else if(my_big_finger(board2) == 0 || opponent_big_finger(board2) == 0) return 2;
  else if(my_big_finger(board3) == 0 || opponent_big_finger(board3) == 0) return 3;
  else if(my_big_finger(board4) == 0 || opponent_big_finger(board4) == 0) return 4;
  else if(my_big_finger(board5) == 0 || opponent_big_finger(board5) == 0) return 5;
  else return 0;
}

/*5つの盤面ハッシュを受け取り、それぞれの盤面に対し2つ以上の行動が存在する場合0を返し、1つのみ(「行動しない」というものも含む)の盤面である場合はそのボードの数を返す*/
unsigned int action_arr_len_1_or_2(unsigned int action_arr_len1,unsigned int action_arr_len2,unsigned int action_arr_len3,unsigned int action_arr_len4,unsigned int action_arr_len5){
  if(action_arr_len1 == 1) return 1;
  else if(action_arr_len2 == 1) return 2;
  else if(action_arr_len3 == 1) return 3;
  else if(action_arr_len4 == 1) return 4;
  else if(action_arr_len5 == 1) return 5;
  else return 0;
}

/*遺伝子を一部突然変異させる*/
void mutate_genome(unsigned int children[GENOME_LENGTH][BOARD_NUM]){
  unsigned int gen1 = my_rand() % 20;
  unsigned int gen2 = my_rand() % 20 + 20;
  unsigned int gen3 = my_rand() % 20 + 40;
  unsigned int gen4 = my_rand() % 20 + 60;
  unsigned int gen5 = my_rand() % 20 + 80;

  
  /*225個の盤面が存在するため、だいたい1%の確率のため二か所の行動インデックスを入れ替える*/
  unsigned int board_1 = my_rand() % BOARD_NUM;
  unsigned int board_2 = my_rand() % BOARD_NUM;
  unsigned int board_3 = my_rand() % BOARD_NUM;
  unsigned int board_4 = my_rand() % BOARD_NUM;
  unsigned int board_5 = my_rand() % BOARD_NUM;
  
  unsigned int action_arr_1[ACTION_CHOICES] = {};
  unsigned int action_arr_len_1 = 0;
  unsigned int action_arr_2[ACTION_CHOICES] = {};
  unsigned int action_arr_len_2 = 0;
  unsigned int action_arr_3[ACTION_CHOICES] = {};
  unsigned int action_arr_len_3 = 0;
  unsigned int action_arr_4[ACTION_CHOICES] = {};
  unsigned int action_arr_len_4 = 0;
  unsigned int action_arr_5[ACTION_CHOICES] = {};
  unsigned int action_arr_len_5 = 0;
  
  action_list(board_1,&action_arr_len_1,action_arr_1);
  action_list(board_2,&action_arr_len_2,action_arr_2);
  action_list(board_3,&action_arr_len_3,action_arr_3);
  action_list(board_4,&action_arr_len_4,action_arr_4);
  action_list(board_5,&action_arr_len_5,action_arr_5);
  unsigned int match_board_flag = match_board(board_1,board_2,board_3,board_4,board_5);
  unsigned int board_exist_flag = board_exist(board_1,board_2,board_3,board_4,board_5);
  unsigned int action_arr_len_flag = action_arr_len_1_or_2(action_arr_len_1,action_arr_len_2,action_arr_len_3,action_arr_len_4,action_arr_len_5);
  while(match_board_flag != 0 || board_exist_flag != 0 || action_arr_len_flag != 0){
    if(match_board_flag == 1 || board_exist_flag == 1 || action_arr_len_flag == 1){board_1 = my_rand() % BOARD_NUM;}
    else if(match_board_flag == 2 || board_exist_flag == 2 || action_arr_len_flag == 2){board_2 = my_rand() % BOARD_NUM;}
    else if(match_board_flag == 3 || board_exist_flag == 3 || action_arr_len_flag == 3){board_3 = my_rand() % BOARD_NUM;}
    else if(match_board_flag == 4 || board_exist_flag == 4 || action_arr_len_flag == 4){board_4 = my_rand() % BOARD_NUM;}
    else if(match_board_flag == 5 || board_exist_flag == 5 || action_arr_len_flag == 5){board_5 = my_rand() % BOARD_NUM;}
    //else {printf("No !!!!!!!!!!!!!!!!!!!!!!!!!\n");}
    //printf("now \n match_board_flag = %d | board_exist_flag = %d || action_arr_len_flag : %d\n",match_board_flag,board_exist_flag,action_arr_len_flag);
    //printf(" board_1 : %d | board_2 : %d | board_3 : %d | board_4 : %d | board_5 : %d\n",board_1,board_2,board_3,board_4,board_5);

    action_list(board_1,&action_arr_len_1,action_arr_1);
    action_list(board_2,&action_arr_len_2,action_arr_2);
    action_list(board_3,&action_arr_len_3,action_arr_3);
    action_list(board_4,&action_arr_len_4,action_arr_4);
    action_list(board_5,&action_arr_len_5,action_arr_5);
    match_board_flag = match_board(board_1,board_2,board_3,board_4,board_5);
    board_exist_flag = board_exist(board_1,board_2,board_3,board_4,board_5);
    action_arr_len_flag = action_arr_len_1_or_2(action_arr_len_1,action_arr_len_2,action_arr_len_3,action_arr_len_4,action_arr_len_5);
    //printf("in mutate_genome, board loop. \n");
  }

  //printf("in mutate_genome, board define. \n");
  
  unsigned int new_action_1 = action_arr_1[my_rand() % action_arr_len_1];
  unsigned int new_action_2 = action_arr_2[my_rand() % action_arr_len_2];
  unsigned int new_action_3 = action_arr_3[my_rand() % action_arr_len_3];
  unsigned int new_action_4 = action_arr_4[my_rand() % action_arr_len_4];
  unsigned int new_action_5 = action_arr_5[my_rand() % action_arr_len_5];
  
  

  /*遺伝子１*/
  /*すでに同じ行動が入っている場合 or 盤面に対しての行動が存在しない*/
  while(children[gen1][board_1] == new_action_1 || children[gen1][board_2] == new_action_2 || children[gen1][board_3] == new_action_3 || children[gen1][board_4] == new_action_4 || children[gen1][board_5] == new_action_5){
    if(children[gen1][board_1] == new_action_1) new_action_1 = action_arr_1[my_rand() % action_arr_len_1];
    else if(children[gen1][board_2] == new_action_2) new_action_2 = action_arr_2[my_rand() % action_arr_len_2];
    else if(children[gen1][board_3] == new_action_3) new_action_3 = action_arr_3[my_rand() % action_arr_len_3];
    else if(children[gen1][board_4] == new_action_4) new_action_4 = action_arr_4[my_rand() % action_arr_len_4];
    else new_action_5 = action_arr_5[my_rand() % action_arr_len_5];
  }

  children[gen1][board_1] = new_action_1;
  children[gen1][board_2] = new_action_2;
  children[gen1][board_3] = new_action_3;
  children[gen1][board_4] = new_action_4;
  children[gen1][board_5] = new_action_5;


  /*遺伝子２*/
  /*すでに同じ行動が入っている場合 or 盤面に対しての行動が存在しない*/
  while(children[gen2][board_1] == new_action_1 || children[gen2][board_2] == new_action_2 || children[gen2][board_3] == new_action_3 || children[gen2][board_4] == new_action_4 || children[gen2][board_5] == new_action_5){
    if(children[gen2][board_1] == new_action_1) new_action_1 = action_arr_1[my_rand() % action_arr_len_1];
    else if(children[gen2][board_2] == new_action_2) new_action_2 = action_arr_2[my_rand() % action_arr_len_2];
    else if(children[gen2][board_3] == new_action_3) new_action_3 = action_arr_3[my_rand() % action_arr_len_3];
    else if(children[gen2][board_4] == new_action_4) new_action_4 = action_arr_4[my_rand() % action_arr_len_4];
    else new_action_5 = action_arr_5[my_rand() % action_arr_len_5];
  }

  children[gen2][board_1] = new_action_1;
  children[gen2][board_2] = new_action_2;
  children[gen2][board_3] = new_action_3;
  children[gen2][board_4] = new_action_4;
  children[gen2][board_5] = new_action_5;

  /*遺伝子３*/
  /*すでに同じ行動が入っている場合 or 盤面に対しての行動が存在しない*/
  while(children[gen3][board_1] == new_action_1 || children[gen3][board_2] == new_action_2 || children[gen3][board_3] == new_action_3 || children[gen3][board_4] == new_action_4 || children[gen3][board_5] == new_action_5){
    if(children[gen3][board_1] == new_action_1) new_action_1 = action_arr_1[my_rand() % action_arr_len_1];
    else if(children[gen3][board_2] == new_action_2) new_action_2 = action_arr_2[my_rand() % action_arr_len_2];
    else if(children[gen3][board_3] == new_action_3) new_action_3 = action_arr_3[my_rand() % action_arr_len_3];
    else if(children[gen3][board_4] == new_action_4) new_action_4 = action_arr_4[my_rand() % action_arr_len_4];
    else new_action_5 = action_arr_5[my_rand() % action_arr_len_5];
  }

  children[gen3][board_1] = new_action_1;
  children[gen3][board_2] = new_action_2;
  children[gen3][board_3] = new_action_3;
  children[gen3][board_4] = new_action_4;
  children[gen3][board_5] = new_action_5;

  /*遺伝子４*/
  /*すでに同じ行動が入っている場合 or 盤面に対しての行動が存在しない*/
  while(children[gen4][board_1] == new_action_1 || children[gen4][board_2] == new_action_2 || children[gen4][board_3] == new_action_3 || children[gen4][board_4] == new_action_4 || children[gen4][board_5] == new_action_5){
    if(children[gen4][board_1] == new_action_1) new_action_1 = action_arr_1[my_rand() % action_arr_len_1];
    else if(children[gen4][board_2] == new_action_2) new_action_2 = action_arr_2[my_rand() % action_arr_len_2];
    else if(children[gen4][board_3] == new_action_3) new_action_3 = action_arr_3[my_rand() % action_arr_len_3];
    else if(children[gen4][board_4] == new_action_4) new_action_4 = action_arr_4[my_rand() % action_arr_len_4];
    else new_action_5 = action_arr_5[my_rand() % action_arr_len_5];
  }

  children[gen4][board_1] = new_action_1;
  children[gen4][board_2] = new_action_2;
  children[gen4][board_3] = new_action_3;
  children[gen4][board_4] = new_action_4;
  children[gen4][board_5] = new_action_5;

  /*遺伝子５*/
  /*すでに同じ行動が入っている場合 or 盤面に対しての行動が存在しない*/
  while(children[gen5][board_1] == new_action_1 || children[gen5][board_2] == new_action_2 || children[gen5][board_3] == new_action_3 || children[gen5][board_4] == new_action_4 || children[gen5][board_5] == new_action_5){
    if(children[gen5][board_1] == new_action_1) new_action_1 = action_arr_1[my_rand() % action_arr_len_1];
    else if(children[gen5][board_2] == new_action_2) new_action_2 = action_arr_2[my_rand() % action_arr_len_2];
    else if(children[gen5][board_3] == new_action_3) new_action_3 = action_arr_3[my_rand() % action_arr_len_3];
    else if(children[gen5][board_4] == new_action_4) new_action_4 = action_arr_4[my_rand() % action_arr_len_4];
    else new_action_5 = action_arr_5[my_rand() % action_arr_len_5];
  }

  children[gen5][board_1] = new_action_1;
  children[gen5][board_2] = new_action_2;
  children[gen5][board_3] = new_action_3;
  children[gen5][board_4] = new_action_4;
  children[gen5][board_5] = new_action_5;




  return;
}

/*genome情報を出力する*/
void print_genome(unsigned int genome[GENOME_LENGTH][BOARD_NUM]){
  for(unsigned int gen = 0; gen < GENOME_LENGTH; gen++){
    if(gen == 30 || gen == 31 || gen == 1){
      printf("==========gen:%d==========\n",gen);
      for(unsigned int board = 0; board < BOARD_NUM; board++){
        printf("board %3d : %d | ",board,genome[gen][board]);
        if(board %10 == 9)printf("\n");
      }
      printf("\n");
    }
  }
  return;
}

/*遺伝情報をcsvに出力する(未完成)*/
/*void output_genome_csv(unsigned int genome[GENOME_LENGTH][BOARD_NUM],char *fname){
  return;
}*/








int main(void){
  unsigned int parents[GENOME_LENGTH][BOARD_NUM]; // (GENOME_LENGTH)個の現行世代(親)個体
  unsigned int children[GENOME_LENGTH][BOARD_NUM]; // (GENOME_LENGTH)個の次世代(子)個体
  judge evaluations[GENOME_LENGTH]; // 現行世代(親)に対応したそれぞれの評価値を登録
  unsigned int opponent[BOARD_NUM]; // 対戦相手のボード情報

  /* (GENOME_LENGTH)個の現行世代の遺伝子個体をランダムに生成 */
  for(unsigned int gen = 0; gen < GENOME_LENGTH; gen ++){
    init_genome_random(children[gen]);
  }

  printf("----------------------------------------first generation(children)---------------------------------------------\n");
  //print_genome(children);

  evaluate_genome(children,opponent,evaluations);
  /*まずは現行世代の中でどれが一番良い個体であるかを捜す*/
  
  unsigned int elite_situ = 0;
  unsigned int elite_position = 0;
  for(int i = 0; i < 10; i++){
    for(unsigned int gen = 0; gen < GENOME_LENGTH; gen++){
      if(elite_situ < evaluations[gen].goalpoints_difference){
        elite_situ = evaluations[gen].goalpoints_difference;
        elite_position = gen;
      }
    }
    printf("==========gen:%d==========\n",elite_position);
    printf("========evaluation:%d=========\n",elite_situ);
    for(unsigned int board = 0; board < BOARD_NUM; board++){
      printf("board %3d : %d | ",board,children[elite_position][board]);
      if(board %10 == 9)printf("\n");
    }
    evaluations[elite_position].goalpoints_difference = 0;
    elite_situ = 0;
    printf("\n");
  }

  /*最初のランダムな遺伝子情報をcsvに出力*/
  //output_genome_csv(children,"first_children.csv");

  /*世代数分loopする*/
  for(unsigned int generation = 1; generation <= GENERATIONS_NUMBER; generation++){

    //printf("generation = %d\n",generation);
    
    /*childrenの値をparents(現行世代)にコピー*/
    copy_genome(parents,children);

    //printf(" copy_genome finish. \n");
    

    
    /*それぞれの個体を評価する*/
    evaluate_genome(parents,opponent,evaluations);

    //printf(" evaluate_genome finish. \n");


    /*現行世代(親)個体から評価規準に基づいて交叉させて、次世代(子)個体を生成する*/
    generate_genome(parents,children,evaluations);
    
    //printf(" generate_genome finish. \n");
    /*
    if(generation == 1){
      printf("----------------------------------------second generation(children)---------------------------------------------\n");
      print_genome(children);
    }*/

    /*次世代(子)を突然変異させる*/
    mutate_genome(children);
    //printf(" mutate_genome finish. \n");
    
    /*次世代(子)遺伝子情報を出力する*/
    //if(generation % 100 == 1){
      //printf("=========generation:%d==========\n",generation);
      //print_genome(children);
      //printf("================================\n");
    //}

  }

  /*最終世代を表示*/
  printf("------------------------------------------final generation--------------------------------------------\n");
  /*それぞれの個体を評価して最終的に一番良い個体を返す*/
  evaluate_genome(children,opponent,evaluations);
  /*unsigned int */elite_situ = 0;
  /*unsigned int */elite_position = 0;
  /*まずは現行世代の中でどれが一番良い個体であるかを捜す*/
  for(int i = 0; i < 100; i++){
    elite_situ = 0;
    elite_position = 0;
    for(unsigned int gen = 0; gen < GENOME_LENGTH; gen++){
      if(elite_situ < evaluations[gen].goalpoints_difference){
        elite_situ = evaluations[gen].goalpoints_difference;
        elite_position = gen;
      }
    }
    printf("==========gen:%d==========\n",elite_position);
    printf("========evaluation:%d=========\n",elite_situ);
    for(unsigned int board = 0; board < BOARD_NUM; board++){
      printf("board %3d : %d | ",board,children[elite_position][board]);
      if(board %10 == 9)printf("\n");
    }
    printf("\n");
    evaluations[elite_position].goalpoints_difference = 0;
  }
  //print_genome(children);

  /*最終世代をcsvに出力*/
  //output_genome_csv(children,"final_children.csv");

  return 0;
}





