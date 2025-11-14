#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 256

// 스택 자료구조 정의 (연산자용)
typedef struct {
  char items[MAX_SIZE];
  int top;
} OperatorStack;

// 스택 자료구조 정의 (피연산자-숫자용)
typedef struct {
  double items[MAX_SIZE];
  int top;
} NumberStack;

// --- 스택 기본 함수 ---
void initOperatorStack(OperatorStack* s) { s->top = -1; }
int isOperatorStackEmpty(OperatorStack* s) { return s->top == -1; }
int isOperatorStackFull(OperatorStack* s) { return s->top == MAX_SIZE - 1; }
void pushOperator(OperatorStack* s, char item) { if (isOperatorStackFull(s)) { printf("오류: 연산자 스택이 꽉 찼습니다.\n"); exit(1); } else s->items[++(s->top)] = item; }
char popOperator(OperatorStack* s) { if (isOperatorStackEmpty(s)) return '\0'; return s->items[(s->top)--]; }
char peekOperator(OperatorStack* s) { if (isOperatorStackEmpty(s)) return '\0'; return s->items[s->top]; }

void initNumberStack(NumberStack* s) { s->top = -1; }
int isNumberStackEmpty(NumberStack* s) { return s->top == -1; }
int isNumberStackFull(NumberStack* s) { return s->top == MAX_SIZE - 1; }
void pushNumber(NumberStack* s, double item) { if (isNumberStackFull(s)) { printf("오류: 숫자 스택이 꽉 찼습니다.\n"); exit(1); } else s->items[++(s->top)] = item; }
double popNumber(NumberStack* s) { if (isNumberStackEmpty(s)) return 0.0; return s->items[(s->top)--]; }

// --- 계산기 핵심 함수 ---

// 연산자 우선순위 반환
int precedence(char op) {
  if (op == '+' || op == '-') return 1;
  if (op == '*' || op == '/') return 2;
  return 0; // 괄호의 경우
}

// 중위 표기법 -> 후위 표기법 변환 함수
void infixToPostfix(const char* infix, char* postfix) {
  OperatorStack s;
  initOperatorStack(&s);
  int i = 0, j = 0;

  while (infix[i] != '\0') {
    char token = infix[i];

    if (isspace(token)) {
      i++;
      continue;
    }

    // 여러 자리 숫자 및 소수점 처리
    if (isdigit(token) || token == '.') {
      while (isdigit(infix[i]) || infix[i] == '.') {
        postfix[j++] = infix[i++];
      }
      postfix[j++] = ' '; // 숫자 구분을 위해 공백 추가
    }
    else if (token == '(') {
      pushOperator(&s, token);
      i++;
    }
    else if (token == ')') {
      while (!isOperatorStackEmpty(&s) && peekOperator(&s) != '(') {
        postfix[j++] = popOperator(&s);
        postfix[j++] = ' ';
      }
      popOperator(&s);
      i++;
    }
    else {
      while (!isOperatorStackEmpty(&s) && precedence(peekOperator(&s)) >= precedence(token)) {
        postfix[j++] = popOperator(&s);
        postfix[j++] = ' ';
      }
      pushOperator(&s, token);
      i++;
    }
  }

  while (!isOperatorStackEmpty(&s)) {
    postfix[j++] = popOperator(&s);
    postfix[j++] = ' ';
  }
  if (j > 0) postfix[j - 1] = '\0';
  else postfix[0] = '\0';
}

// 후위 표기법 계산 함수
double evaluatePostfix(char* postfix) {
  char postfix_copy[MAX_SIZE];
  strncpy(postfix_copy, postfix, MAX_SIZE);

  NumberStack s;
  initNumberStack(&s);
  char* token = strtok(postfix_copy, " ");

  while (token != NULL) {
    if (isdigit(token[0]) || token[0] == '.' || (token[0] == '-' && isdigit(token[1]))) {
      pushNumber(&s, atof(token));
    }
    else if (strlen(token) == 1) {
      double val2 = popNumber(&s);
      double val1 = popNumber(&s);
      switch (token[0]) {
      case '+': pushNumber(&s, val1 + val2); break;
      case '-': pushNumber(&s, val1 - val2); break;
      case '*': pushNumber(&s, val1 * val2); break;
      case '/':
        if (val2 == 0) {
          printf("오류: 0으로 나눌 수 없습니다.\n");
          return 0.0;
        }
        pushNumber(&s, val1 / val2);
        break;
      }
    }
    token = strtok(NULL, " ");
  }

  // 최종 결과를 반환하기 전 스택이 비어있는지 확인
  if (isNumberStackEmpty(&s)) {
    printf("오류: 수식이 잘못되었습니다.\n");
    return 0.0;
  }
  return popNumber(&s);
}

int main() {
  char infix[MAX_SIZE];
  char postfix[MAX_SIZE] = { 0 };

  printf("계산할 수식을 입력하세요: ");
  if (fgets(infix, MAX_SIZE, stdin) == NULL) {
    return 1;
  }

  infix[strcspn(infix, "\n")] = 0;

  infixToPostfix(infix, postfix);
  if (strlen(postfix) == 0) {
    printf("유효하지 않은 수식입니다.\n");
    return 1;
  }

  printf("후위 표기법 변환: %s\n", postfix);

  double result = evaluatePostfix(postfix);
  printf("계산 결과: %g\n", result);

  return 0;
}