/*
 * Copyright 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/*
 * 기본 스도쿠 퍼즐
 */
int sudoku[9][9] =
{{6,3,9,8,4,1,2,7,5},
{7,2,4,9,5,3,1,6,8},
{1,8,5,7,2,6,3,9,4},
{2,5,6,1,3,7,4,8,9},
{4,9,1,5,8,2,6,3,7},
{8,7,3,4,6,9,5,2,1},
{5,4,2,3,9,8,7,1,6},
{3,1,8,6,7,5,9,4,2},
{9,6,7,2,1,4,8,5,3}};

/*
 * valid[0][0], valid[0][1], ..., valid[0][8]: 각 행이 올바르면 1, 아니면 0
 * valid[1][0], valid[1][1], ..., valid[1][8]: 각 열이 올바르면 1, 아니면 0
 * valid[2][0], valid[2][1], ..., valid[2][8]: 각 3x3 그리드가 올바르면 1, 아니면 0
 */
int valid[3][9];

/*
 * 스도쿠 퍼즐의 각 행이 올바른지 검사한다.
 * 행 번호는 0부터 시작하며, i번 행이 올바르면 valid[0][i]에 1을 기록한다.
 */

void *check_rows(void *arg){
  int rowIs=1;//rows의 결과
  int tmp;

  for (int i = 0; i < 9; i++){
    /*
    *i번째 rows에서 1~9의 숫자가 모두 있는지 확인하기 위해 row배열을 0으로 초기화한다.
    */
    int row[9]={0,};
    for (int j = 0; j < 9; j++){
      tmp = sudoku[i][j];

      /*
      *1~9의 숫자가 들어있다면, 자리를 맞춰주기 위해 -1을 해주고, 그 자리에 1을 대입해준다.
      *{0,1,2}의 순으로 배열이 되므로 1~3에서 -1을 해줘야 숫자와 자리가 알맞게 들어갈 수 있다.
      */
      if(row[tmp-1]==0){
        row[tmp-1]=1;
      }

      /*
      *1~9의 숫자가 이미 존재한다면 0이 아닌 1이 대입되어 있을 것이므로,
      *row[tmp-1]이 0이 아닐 경우 i번째 rows의 결과는 0이 된다.
      */
      else{
        rowIs =0;
      }
    }
    /*
    *해당 결과값을 valid[0][i]에 대입하여 준 뒤,
    *i+1번째 결과를 확인하기 위해 rows는 초기화 시켜준다.
    */
    valid[0][i]=rowIs;
    rowIs = 1;
  }
  pthread_exit(NULL);
}

/*
 * 스도쿠 퍼즐의 각 열이 올바른지 검사한다.
 * 열 번호는 0부터 시작하며, j번 열이 올바르면 valid[1][j]에 1을 기록한다.
 */
void *check_columns(void *arg){
  int colIs=1;//column의 결과값
  int tmp;

  for(int i=0; i<9; i++){
    /*
    *i번째 column에서 1~9의 숫자가 모두 있는지 확인하기 위해 col배열을 0으로 초기화한다.
    */
    int col[9]={0,};
    for(int j=0; j<9; j++){
      tmp = sudoku[j][i];

    /*
    *1~9의 숫자가 들어있다면, 자리를 맞춰주기 위해 -1을 해주고, 그 자리에 1을 대입해준다.
    *{0,1,2}의 순으로 배열이 되므로 1~3에서 -1을 해줘야 숫자와 자리가 알맞게 들어갈 수 있다.
    */
    if(col[tmp-1]==0){
        col[tmp-1]=1;
      }

      /*
      *1~9의 숫자가 이미 존재한다면 0이 아닌 1이 대입되어 있을 것이므로,
      *col[tmp-1]이 0이 아닐 경우 i번째 column의 결과는 0이 된다.
      */
      else{
        colIs=0;
      }
    }
    /*
    *해당 결과값을 valid[1][i]에 대입하여 준 뒤,
    *i+1번째 결과를 확인하기 위해 colIs는 초기화 시켜준다.
    */
    valid[1][i]=colIs;
    colIs=1;
  }
  pthread_exit(NULL);
}
/*
 * 스도쿠 퍼즐의 각 3x3 서브그리드가 올바른지 검사한다.
 * 3x3 서브그리드 번호는 0부터 시작하며, 왼쪽에서 오른쪽으로, 위에서 아래로 증가한다.
 * k번 서브그리드가 올바르면 valid[2][k]에 1을 기록한다.
 */
void *check_subgrid(void *arg){
  int subIs=1;
  int tmp;

  /*
  * 0~8번째 서브그리드를 확인하기 위한 범위를 설정해준다.
  * 예를 들어, 0~2번째 서브그리드는  0~2번째 행이지만 0~2,3~5,6~8번째 열이기 때문에
  * 행은 몫을 기준으로, 열은 나머지를 기준으로 범위를 설정한다.
  */
  int subgrids = *((int*)arg);
  int rmin = (subgrids/3)*3;
  int rmax = rmin+3;
  int cmin = (subgrids%3)*3;
  int cmax = cmin+3;

  int sub[9]={0,};

  for(int i=rmin; i<rmax; i++){
    for(int j=cmin; j<cmax; j++){
      tmp = sudoku[i][j];

      /*
      *1~9의 숫자가 들어있다면, 자리를 맞춰주기 위해 -1을 해주고, 그 자리에 1을 대입해준다.
      *{0,1,2}의 순으로 배열이 되므로 1~3에서 -1을 해줘야 숫자와 자리가 알맞게 들어갈 수 있다.
      */
      if(sub[tmp-1]==0){
        sub[tmp-1]=1;
      }

      /*
      *1~9의 숫자가 이미 존재한다면 0이 아닌 1이 대입되어 있을 것이므로,
      *col[tmp-1]이 0이 아닐 경우 해당 subgrid의 결과는 0이 된다.
      */
      else{
        subIs=0;
      }
    }
  }

  /*
  * 해당 결과값을 valid[2][i]에 대입하여 준 뒤,
  * 서브그리드의 결과값이 중복되어 들어가지 않도록 초기화 시켜준다.
  */
  valid[2][subgrids]=subIs;
  pthread_exit(NULL);
}

/*
 * 스도쿠 퍼즐이 올바르게 구성되어 있는지 11개의 스레드를 생성하여 검증한다.
 * 한 스레드는 각 행이 올바른지 검사하고, 다른 한 스레드는 각 열이 올바른지 검사한다.
 * 9개의 3x3 서브그리드에 대한 검증은 9개의 스레드를 생성하여 동시에 검사한다.
 */
void check_sudoku(void)
{
    int i, j;

    /*
     * 검증하기 전에 먼저 스도쿠 퍼즐의 값을 출력한다.
     */
    for (i = 0; i < 9; ++i) {
        for (j = 0; j < 9; ++j)
            printf("%2d", sudoku[i][j]);
        printf("\n");
    }
    printf("---\n");
    /*
     * 스레드를 생성하여 각 행을 검사하는 check_rows() 함수를 실행한다.
     */
    pthread_t sudoku[11];

    pthread_create(&sudoku[0],NULL,check_rows,NULL);
    /*
     * 스레드를 생성하여 각 열을 검사하는 check_columns() 함수를 실행한다.
     */
    pthread_create(&sudoku[1],NULL,check_columns,NULL);
    /*
     * 9개의 스레드를 생성하여 각 3x3 서브그리드를 검사하는 check_subgrid() 함수를 실행한다.
     * 3x3 서브그리드의 위치를 식별할 수 있는 값을 함수의 인자로 넘긴다.
     */
    int subgrid[9]={0,1,2,3,4,5,6,7,8};
    pthread_create(&sudoku[2],NULL,check_subgrid,(void*)&subgrid[0]);
    pthread_create(&sudoku[3],NULL,check_subgrid,(void*)&subgrid[1]);
    pthread_create(&sudoku[4],NULL,check_subgrid,(void*)&subgrid[2]);
    pthread_create(&sudoku[5],NULL,check_subgrid,(void*)&subgrid[3]);
    pthread_create(&sudoku[6],NULL,check_subgrid,(void*)&subgrid[4]);
    pthread_create(&sudoku[7],NULL,check_subgrid,(void*)&subgrid[5]);
    pthread_create(&sudoku[8],NULL,check_subgrid,(void*)&subgrid[6]);
    pthread_create(&sudoku[9],NULL,check_subgrid,(void*)&subgrid[7]);
    pthread_create(&sudoku[10],NULL,check_subgrid,(void*)&subgrid[8]);

    /*
     * 11개의 스레드가 종료할 때까지 기다린다.
     */
    for(i=0; i<11; i++){
      pthread_join(sudoku[i],NULL);
    }

    /*
     * 각 행에 대한 검증 결과를 출력한다.
     */
    printf("ROWS: ");
    for (i = 0; i < 9; ++i)
        printf(valid[0][i] == 1 ? "(%d,YES)" : "(%d,NO)", i);
    printf("\n");
    /*
     * 각 열에 대한 검증 결과를 출력한다.
     */
    printf("COLS: ");
    for (i = 0; i < 9; ++i)
        printf(valid[1][i] == 1 ? "(%d,YES)" : "(%d,NO)", i);
    printf("\n");
    /*
     * 각 3x3 서브그리드에 대한 검증 결과를 출력한다.
     */
    printf("GRID: ");
    for (i = 0; i < 9; ++i)
        printf(valid[2][i] == 1 ? "(%d,YES)" : "(%d,NO)", i);
    printf("\n---\n");
}

/*
 * 스도쿠 퍼즐의 값을 3x3 서브그리드 내에서 무작위로 섞는 함수이다.
 */
void *shuffle_sudoku(void *arg)
{
    int i, tmp;
    int grid;
    int row1, row2;
    int col1, col2;

    srand(time(NULL));
    for (i = 0; i < 100; ++i) {
        /*
         * 0부터 8번 사이의 서브그리드 하나를 무작위로 선택한다.
         */
        grid = rand() % 9;
        /*
         * 해당 서브그리드의 좌측 상단 행열 좌표를 계산한다.
         */
        row1 = row2 = (grid/3)*3;
        col1 = col2 = (grid%3)*3;
        /*
         * 해당 서브그리드 내에 있는 임의의 두 위치를 무작위로 선택한다.
         */
        row1 += rand() % 3; col1 += rand() % 3;
        row2 += rand() % 3; col2 += rand() % 3;
        /*
         * 홀수 서브그리드이면 두 위치에 무작위 수로 채우고,
         */
        if (grid & 1) {
            sudoku[row1][col1] = rand() % 8 + 1;
            sudoku[row2][col2] = rand() % 8 + 1;
        }
        /*
         * 짝수 서브그리드이면 두 위치에 있는 값을 맞바꾼다.
         */
        else {
            tmp = sudoku[row1][col1];
            sudoku[row1][col1] = sudoku[row2][col2];
            sudoku[row2][col2] = tmp;
        }
    }
    pthread_exit(NULL);
}

/*
 * 메인 함수는 위에서 작성한 함수가 올바르게 동작하는지 검사하기 위한 것으로 수정하면 안 된다.
 */
int main(void)
{
    int tmp;
    pthread_t tid;

    /*
     * 기본 스도쿠 퍼즐을 출력하고 검증한다.
     */
    check_sudoku();
    /*
     * 기본 퍼즐에서 값 두개를 맞바꾸고 검증해본다.
     */
    tmp = sudoku[5][3]; sudoku[5][3] = sudoku[6][2]; sudoku[6][2] = tmp;
    check_sudoku();
    /*
     * 기본 스도쿠 퍼즐로 다시 바꾼 다음, shuffle_sudoku 스레드를 생성하여 퍼즐을 섞는다.
     */
    tmp = sudoku[5][3]; sudoku[5][3] = sudoku[6][2]; sudoku[6][2] = tmp;
    if (pthread_create(&tid, NULL, shuffle_sudoku, NULL) != 0) {
        fprintf(stderr, "pthread_create error: shuffle_sudoku\n");
        exit(-1);
    }
    /*
     * 무작위로 섞는 중인 스도쿠 퍼즐을 검증해본다.
     */
    check_sudoku();
    /*
     * shuffle_sudoku 스레드가 종료될 때까지 기다란다.
     */
    pthread_join(tid, NULL);
    /*
     * shuffle_sudoku 스레드 종료 후 다시 한 번 스도쿠 퍼즐을 검증해본다.
     */
    check_sudoku();
    exit(0);
}
