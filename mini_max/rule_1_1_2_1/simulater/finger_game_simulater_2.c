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

int main(void){
  int action = 0;
  int turn_num = 0;
  int board = 80;
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
      if(turn_num % 2 == 1) printf("next your turn. please input action.\n");
      else if(turn_num % 2 == 0) printf("next cpu turn. please input action.\n");
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
      if(turn_num % 2 == 0) printf("next your turn. please input action.\n");
      else if(turn_num % 2 == 1) printf("next cpu turn. please input action.\n");
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