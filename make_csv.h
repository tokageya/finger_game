/*=====      make_csv.h      =====*/



/*make_csv関数*/
// csvファイルを作成する。C99指定しないと使えない。
// それぞれの引数の意味は以下の通りである。
//
// fname … 作成するファイルの名前の文字列。「newton.csv」など。
// line_num … 行数
// line_names … 行の文字列の配列(char型の二次元配列)。line_numが要素数。
// column_num … 列数
// column_names … 列の文字列の配列(char型の二次元配列)。column_numが要素数。
// elements … 要素が入っているint型の二次元配列。

#define MAX_STRING_LEN 20

void make_csv(char *fname, int line_num, char line_names[line_num][MAX_STRING_LEN],int column_num, char column_names[column_num][MAX_STRING_LEN], double elements[line_num][column_num]){
    FILE *fp;
    fp = fopen( fname, "w" );
    if( fp == NULL ){
        printf( "%sファイルが開けません\n", fname );
        //exit(1);
        return;
    }

    /*----- 0行目 -----*/
    //最初の行は列の名前
    if(column_num > 0){
        fprintf( fp,",");
        for(int j = 0; j < (column_num-1); j++){
            fprintf( fp, "%s,", column_names[j]);
        }
        fprintf(fp, "%s\n", column_names[(column_num-1)]);
    }

    /*----- 1行目以降 -----*/
    for(int i = 0; i < line_num; i++){
        fprintf(fp,"%s,",line_names[i]);
        for(int j = 0; j < (column_num-1); j ++){
            fprintf( fp, "%f,",elements[i][j]);
        }
        fprintf(fp,"%f\n",elements[i][(column_num-1)]);
    }

    fclose( fp );

    printf( "%sファイル書き込みが終わりました\n", fname );
    return;
}

