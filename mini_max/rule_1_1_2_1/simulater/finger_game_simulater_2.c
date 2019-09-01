#include<stdio.h>
#include<stdlib.h>

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

#define ACTIOIN_CHOICES 10 // 行動インデックスの総数


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

/*盤面ハッシュを一時的に相手視点に変えるための関数*/
unsigned int reverse_board(unsigned int board){
  unsigned int board2 = (opponent_hands_index(board) * 15) + my_hands_index(board);
  return board2;
}

/*引数の盤面ハッシュに対応する盤面から、引数の行動インデックスに対応する行動をとると遷移する次の盤面を返す*/
unsigned int next_board(unsigned int now_board, unsigned int action){
  unsigned int player1_s = my_small_finger(now_board);
  unsigned int player1_b = my_big_finger(now_board);
  unsigned int player2_s = opponent_small_finger(now_board);
  unsigned int player2_b = opponent_big_finger(now_board);
  //unsigned int board = now_board;
  /*行動インデックス毎に場合分け*/
  if(action == 1){
    player2_b += player1_b;
  }
  else if(action == 2){
    if(player2_s == 0) player2_b += player1_b;
    else player2_s += player1_b;
  }
  else if(action == 3){
    if(player1_s == 0) player2_b += player1_b;
    else player2_b += player1_s;
  }
  else if(action == 4){
    if(player1_s == 0 && player2_s == 0) player2_b += player1_b;
    else if(player1_s == 0) player2_s += player1_b;
    else if(player2_s == 0) player2_b += player1_s;
    else player2_s += player1_s;
  }
  else if(action == 5){
    player1_s += player1_b;
  }
  else if(action == 6){
    player1_b += player1_s;
  }
  else if(action == 7){
    unsigned int sum = player1_b + player1_s;
    player1_s = 1;
    player1_b = sum-1;
  }
  else if(action == 8){
    unsigned int sum = player1_b + player1_s;
    player1_s = 2;
    player1_b = sum - 2;
    
  }
  else if(action == 9){
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

/*盤面の状況を人が見やすいように出力する*/
void print_board(unsigned int board){
  printf("=======board:%3d(reverse:%3d)======\n",board,reverse_board(board));
  printf("   cpu\n");
  printf("( %d , %d )\n",opponent_small_finger(board),opponent_big_finger(board));
  printf("\n\n");
  printf("( %d , %d )\n",my_small_finger(board),my_big_finger(board));
  printf("   player\n");
  printf("===================================\n");
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

int main(void){
  unsigned int action = 0;
  unsigned int turn_num = 0;
  unsigned int board = 80;
  unsigned int action_list_arr[10];
  unsigned int action_list_arr_len;
  printf("Please input (f or d),(board number).\n");
  char c;
  while(1){
    scanf("%c,%d",&c,&board);
    if(c == 'f') {printf("You are first turn.\n");break;}
    else if(c == 'd') {printf("You are draw turn.\n");break;}
    else printf("Error. Please input again.\n");
  }
  printf("board = %d\n",board);
  if(c == 'f'){
    print_board(board);
    while(my_big_finger(board) != 0 && opponent_big_finger(board) != 0){
      /*ターンを進める*/
      turn_num++;
      if(turn_num % 2 == 1){
        printf("next your turn. please input action.\n");
        action_list(board,&(action_list_arr_len),action_list_arr);
      }
      else if(turn_num % 2 == 0){ 
        printf("next cpu turn. please input action.\n");
        action_list(reverse_board(board),&(action_list_arr_len),action_list_arr);
      }
      printf("now action_list : [");
      for(int i=0; i<action_list_arr_len; i++) printf("%d, ", action_list_arr[i]);
      printf("]\n");
      scanf("%d",&action);
      //printf("read %d.\n",action);
      //if(/*盤面に対して打てる手が入力されていなかった場合*/){}
      //else{
        if(turn_num % 2 == 1) board = next_board(board, action);
        else if(turn_num % 2 == 0){
          unsigned int board2 = reverse_board(board);//相手視点の盤面ハッシュ
          board = reverse_board( next_board( board2, action ) );
        }
        print_board(board);
      //}
    }
  }
  else if(c == 'd'){
    print_board(board);
    while(my_big_finger(board) != 0 && opponent_big_finger(board) != 0){
      /*ターンを進める*/
      turn_num++;
      if(turn_num % 2 == 0){
        printf("next your turn. please input action.\n");
        action_list(board,&(action_list_arr_len),action_list_arr);
      }
      else if(turn_num % 2 == 1){ 
        printf("next cpu turn. please input action.\n");
        action_list(reverse_board(board),&(action_list_arr_len),action_list_arr);
      }
      printf("now action_list : [");
      for(int i=0; i<action_list_arr_len; i++) printf("%d, ", action_list_arr[i]);
      printf("]\n");
      scanf("%d",&action);
      //printf("read %d.\n",action);
      if(turn_num % 2 == 0) board = next_board(board, action);
      else if(turn_num % 2 == 1){
        unsigned int board2 = reverse_board(board);//相手視点の盤面ハッシュ
        board = reverse_board( next_board( board2, action ) );
      }
      print_board(board);
    } 
  }
  printf("\nfinish.\n");
  if(my_big_finger(board) == 0) printf("You lose.\n");
  else  printf("You win.\n");
  return 0;
}