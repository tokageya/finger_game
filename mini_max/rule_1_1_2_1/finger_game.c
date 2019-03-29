
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

/*方針*/

//盤面評価方法
//0. 盤面をまだ探索していないとき OR 存在しない盤面であるとき、　　　　　　　　評価値　０　とする
//1. 勝ちが確定した盤面 OR 勝ちが確定した盤面に任意に移動することができるとき、評価値　１　とする
//2. 負けが確定した盤面 OR 負けが確定した盤面に任意に移動することができるとき、評価値　２　とする
//3. 盤面がループしてしまったとき、　　　　　　　　　　　　　　　　　　　　　　評価値　３　とする

#include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <unistd.h>

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

#define my_turn 0 // 自分のターン
#define opponent_turn 1 // 相手のターン
#define all_user_num 2 // ゲームでの全てのユーザー数

/*ループ判定で使う Trun 構造体を作成*/
typedef struct _Turn{
  unsigned int user;
  unsigned int board;
} Turn;

unsigned int me_count=0;
unsigned int opponent_count=0;


//////=====盤面の符号化・復号化関数を遺伝的アルゴリズムのコードから引用=====/////

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


/*引数の盤面ハッシュに対応する盤面から、引数の行動インデックスに対応する行動をとると遷移する次の盤面を返す*/
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

/*引数の盤面a_boardが、引数の盤面のログに存在するかどうかを確認する*/
/*ループしていたら 1 を返す*/
unsigned int loop_check(unsigned int a_board, unsigned int user, unsigned int board_log_len, Turn* board_log){
  //printf("now        a_board = %2d | user = %2d\n----------------------\n",a_board,user);
  for(unsigned int turn=0; turn < board_log_len; turn++){
    //printf("turn=%2d , board_log[%2d].board = %2d | board_log[%2d].user=%2d\n",turn,turn,board_log[turn].board,turn,board_log[turn].user);
    if(a_board == board_log[turn].board && user == board_log[turn].user) return 1;
  }
  //printf("\n^^^^^^^^^^^^^^^^^\n");
  return 0;
}

/*引数の盤面のログからループしていないかどうかを確認する*/
/*ループしていたら 1 を返す*/
unsigned int loop_check2(unsigned int board_log_len, unsigned int* board_log){
  for(unsigned int turn = 0; turn < board_log_len; turn ++){
    unsigned int a_board = board_log[turn];
    for(unsigned int i = turn+1; i < board_log_len; i++){
      if(a_board == board_log[i]) return 1;
    }
  }
  return 0;
}


/*盤面ハッシュを一時的に相手視点に変えるための関数*/
unsigned int reverse_board(unsigned int board){
  unsigned int board2 = (opponent_hands_index(board) * 15) + my_hands_index(board);
  return board2;
}

//////=====ここからミニマックス法オリジナルの関数定義=====/////

/*探索を始める前に、min_max配列全てに0を格納*/
void init_move(unsigned int b[all_user_num][BOARD_NUM]){
  for(unsigned int user = 0; user < all_user_num; user ++){
    for(unsigned int board = 0; board < BOARD_NUM; board ++){
        /*盤面に0を格納*/
        b[ user ][ board ] = 0;
    }
  }
}

/*評価配列の中から一番適した評価を書く*/
unsigned int select_evaluation(unsigned int user, unsigned int* best_behavior, unsigned int* action_arr, unsigned int action_arr_len, unsigned int* local_board_evaluations, unsigned int local_board_evaluations_len){
  (*best_behavior) = action_arr[0];
  if(user==my_turn){
    unsigned int max_evaluation = local_board_evaluations[0];
    for(unsigned int i=0; i<action_arr_len; i++){
      if( local_board_evaluations[i] == 1 ){
        max_evaluation = local_board_evaluations[i];
        (*best_behavior) = action_arr[i];
        return max_evaluation;
      }
      else if(local_board_evaluations[i] == 2){
        if((*best_behavior)==0) (*best_behavior) = action_arr[i];
      }
      else if(local_board_evaluations[i] == 3 && max_evaluation == 2){
        max_evaluation = local_board_evaluations[i];
        (*best_behavior) = action_arr[i];
      }
      //else printf("\nWorning!! ちゃんとlocal_board_evaluations配列を管理できていないぞ！\n");
    }
    return max_evaluation;
  }
  else{
    unsigned int min_evaluation = local_board_evaluations[0];
    for(unsigned int i=0; i<action_arr_len; i++){
      if( local_board_evaluations[i] == 1 ){
        if((*best_behavior)==0) (*best_behavior) = action_arr[i];
      }
      else if( local_board_evaluations[i] == 2 ){
        min_evaluation = local_board_evaluations[i];
        (*best_behavior) = action_arr[i];
        return min_evaluation;
      }
      else if( local_board_evaluations[i] == 3 && min_evaluation == 1 ){
        min_evaluation = local_board_evaluations[i];
        (*best_behavior) = action_arr[i];
      }
      //else printf("\nWorning!! ちゃんとlocal_board_evaluations配列を管理できていないぞ！\n");
    }
    return min_evaluation;
  }
}





unsigned int opponent(unsigned int min_max[all_user_num][BOARD_NUM],unsigned int board, unsigned int board_evaluations[all_user_num][BOARD_NUM], unsigned int* board_log_len, Turn* board_log);

/*自分のターンのときに指す方法を探る。*/
/*自分の番の時は、その局面の次に出現するすべての局面のうち最も良い評価の手を打つことができるので、
 *次に出現するすべての局面の評価値の最大値を局面の評価値にすればよい。*/
unsigned int me(unsigned int min_max[all_user_num][BOARD_NUM],unsigned int board, unsigned int board_evaluations[all_user_num][BOARD_NUM], unsigned int* board_log_len, Turn* board_log){
  unsigned int my_index = my_hands_index(board);//自分の両手の情報を入れる
  unsigned int opponent_index = opponent_hands_index(board);//相手の両手の情報を入れる
  printf("me関数が呼び出されたゾ\nboard=%d, board_log_len=%d\n",board,*(board_log_len));


  me_count++;
  printf("me_count=%d\n",me_count);
  //sleep(1);


  /*引数の盤面が終わりの状態のとき、勝敗情報を返す*/
  if(opponent_index == 0){
    //board_evaluations[board] = 1;
    printf("勝ったぞ！\n");
    me_count--;
    printf("me_count=%d\n",me_count);
    return 1;
  }
  else if(my_index == 0){
    //board_evaluations[board] = 2;
    printf("負けたぞ！\n");
    me_count--;
    printf("me_count=%d\n",me_count);
    return 2;
  }
  /*引数の盤面が現れている場合、ループ情報を返す*/
  else if(loop_check(board,my_turn,*(board_log_len),board_log)){
    //board_evaluations[board] = 3;
    //printf("loopを検出\nturn=me, board=%d\n",board);
    //sleep(1);
    printf("ループしたぞ\n");
    me_count--;
    printf("me_count=%d\n",me_count);
    return 3;
  }

  /*board_logに行動を追加する*/
  Turn l; 
  l.user = my_turn; 
  l.board = board; 
  board_log[(*(board_log_len))] = l; 
  (*(board_log_len))++;
  /*
  for(unsigned int i=0; i<(*board_log_len);i++){
    printf("turn%d(turn:%d) : %d\n",i,board_log[i].user,board_log[i].board);
  }
  printf("==========================================================\n");
  sleep(1);
  */

  /*そうじゃないときは引数の盤面に対してのそれぞれを打ったときのopponent関数を呼び出す*/
  unsigned int action_arr[ACTION_CHOICES];
  unsigned int action_arr_len;

  action_list(board,&action_arr_len,action_arr);//action_arrの更新

  unsigned int local_board_evaluations[action_arr_len];
  unsigned int local_board_evaluations_len = action_arr_len;
  
  //printf("in me関数\n");
  for(int i=0; i<local_board_evaluations_len; i++){
    local_board_evaluations[i] = opponent(min_max/*[all_user_num][BOARD_NUM]*/,next_board(board,action_arr[i]),board_evaluations/*[all_user_num][BOARD_NUM]*/,board_log_len, board_log);
    //printf("local_board_evaluations[%d] = %d\n",i,local_board_evaluations[i]);
  }
  //sleep(2);
  

  /*board_logに追加した要素を消す*/
  (*(board_log_len)) --;

  /*一番マシな評価値を返す*/
  unsigned int best_behavior;
  unsigned int now_evaluation = select_evaluation(my_turn, &best_behavior, action_arr, action_arr_len, local_board_evaluations, local_board_evaluations_len);
  min_max[my_turn][board] = best_behavior;
  //board_evaluations[my_turn][board] = now_evaluation;
  me_count--;
  printf("me関数の評価をし終えたゾ\nme_count=%d\n",me_count);
  //sleep(1);
  return now_evaluation;

}

/*相手のターンのときに指す方法を探る*/
/*相手の番の時は最も有利になる手を打ってくるはずであるため、
 *次に出現するすべての局面の評価値の最小値を局面の評価値にすればよい*/
unsigned int opponent(unsigned int min_max[all_user_num][BOARD_NUM],unsigned int board, unsigned int board_evaluations[all_user_num][BOARD_NUM], unsigned int* board_log_len, Turn* board_log){
  unsigned int my_index = my_hands_index(reverse_board(board) );//自分の両手の情報を入れる
  unsigned int opponent_index = opponent_hands_index(reverse_board(board));//相手の両手の情報を入れる

  printf("opponent関数が呼び出されたゾ\nboard=%d, board_log_len=%d\n",board,*(board_log_len));

  opponent_count++;
  printf("opponent_count=%d\n",opponent_count);
  //sleep(1);
  
  /*引数の盤面が終わりの状態のとき、勝敗情報を返す*/
  if(opponent_index == 0){
    //board_evaluations[board] = 1;
    printf("負けたぞ！\n");
  
    opponent_count--;
    
    printf("opponent_count=%d\n",opponent_count);
    return 1;
  }
  else if(my_index == 0){
    //board_evaluations[board] = 2;
    printf("勝ったぞ！\n");
    opponent_count--;
    
    printf("opponent_count=%d\n",opponent_count);
    return 2;
  }
  /*引数の盤面が現れている場合、ループ情報を返す*/
  else if(loop_check(board,opponent_turn,*(board_log_len),board_log)){
    //board_evaluations[board] = 3;
    //printf("loopを検出\nturn=opponent, board=%d\n",board);
    printf("ループしたぞ\n");
    opponent_count--;
    printf("opponent_count=%d\n",opponent_count);
    //sleep(1);
    return 3;
  }

  /*board_logに行動を追加する*/
  Turn l; 
  l.user = opponent_turn; 
  l.board = board; 
  board_log[(*(board_log_len))] = l; 
  (*(board_log_len))++;

  /*そうじゃないときは引数の盤面に対してのそれぞれを打ったときのopponent関数を呼び出す*/
  unsigned int action_arr[ACTION_CHOICES];
  unsigned int action_arr_len;

  /*相手のため、盤面情報を反転させてからaction_arr配列を更新する*/

  action_list(reverse_board(board),&action_arr_len,action_arr);//action_arrの更新

  unsigned int local_board_evaluations[action_arr_len];
  unsigned int local_board_evaluations_len = action_arr_len;

  //printf("in opponent関数\n");
  for(int i=0; i<action_arr_len; i++){
    local_board_evaluations[i] = me(min_max/*[all_user_num][BOARD_NUM]*/,reverse_board(next_board(reverse_board(board),action_arr[i])),board_evaluations/*[all_user_num][BOARD_NUM]*/,board_log_len,board_log);
    //printf("local_board_evaluations[%d] = %d\n",i,local_board_evaluations[i]);
  }
  //sleep(2);

  /*board_logに追加した要素を消す*/
  (*(board_log_len)) --;

  /*一番マシな評価値を返す*/
  unsigned int best_behavior;
  unsigned int now_evaluation = select_evaluation(opponent_turn, &best_behavior, action_arr, action_arr_len, local_board_evaluations, local_board_evaluations_len);
  min_max[opponent_turn][board] = best_behavior;
  //board_evaluations[opponent_turn][board] = now_evaluation;
  opponent_count--;
  printf("opponent関数の評価をし終えたゾ\nopponent_count=%d\n",opponent_count);
  //sleep(1);
  return now_evaluation;
}

/*ミニマックス探索による情報を出力する*/
void print_min_max(unsigned int min_max[all_user_num][BOARD_NUM]){
  for(unsigned int user=0; user<all_user_num; user++){
    if(user==my_turn) printf("================================my_turn==================================\n");
    else printf("================================opponent_turn==================================\n");
    for(unsigned int board=0; board<BOARD_NUM; board++){
      printf("board %d : %d",board,min_max[user][board]);
      if(board%10==9)printf("\n");
    }
  }
}


int main(void){
  Turn board_log[MAX_LOG];//盤面のログ情報を格納
  unsigned int board_log_len = 0;//盤面のログ情報のターン数
  unsigned int board_evaluations[all_user_num][BOARD_NUM];//みにまっくす
  unsigned int min_max[all_user_num][BOARD_NUM];//ミニマックスによる盤面での行動を格納
  
  /*ミニマックスの配列を初期化*/
  init_move(min_max/*[all_user_num][BOARD_NUM]*/);
  /*===とりあえず自分が先攻のときのミニマックスを探索する===*/

  /*ミニマックスによる探索*/
  board_evaluations[my_turn][80] = me(min_max/*[all_user_num][BOARD_NUM]*/,80,board_evaluations/*[all_user_num][BOARD_NUM]*/,&board_log_len,board_log);

  /*ミニマックスによる結果を表示*/
  print_min_max(min_max/*[all_user_num][BOARD_NUM]*/);

  return 0;
}