#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

// Oyun alaný boyutlarý
#define GRID_SIZE 30 // 30x30 grid (büyüttük)
#define CELL_SIZE 20 // Her kare 20 piksel
#define WINDOW_SIZE (GRID_SIZE * CELL_SIZE) // Pencere boyutu: 600x600

// Yýlan ve yem için yapýlar
typedef struct {
    int x, y; // Koordinatlar
} Point;

Point snake[100]; // Yýlanýn gövdesi (maks 100 segment)
int snakeLength = 3; // Baþlangýç uzunluðu
Point food; // Yem konumu
int direction = 1; // 0: yukarý, 1: sað, 2: aþaðý, 3: sol
bool gameOver = false;
int score = 0;

// Baþlangýç ayarlarý
void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0); // Arka plan siyah
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, WINDOW_SIZE, 0, WINDOW_SIZE); // 2D grid: 600x600
    srand(time(NULL)); // Rastgele sayý için seed
    // Yýlanýn baþlangýç konumu (gridin ortasýna yakýn)
    snake[0].x = 15; snake[0].y = 15; // Ortada baþla
    snake[1].x = 14; snake[1].y = 15;
    snake[2].x = 13; snake[2].y = 15;
    // Ýlk yemi yerleþtir
    food.x = rand() % GRID_SIZE;
    food.y = rand() % GRID_SIZE;
}

// Kare çizme fonksiyonu
void drawSquare(int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2i(x * CELL_SIZE, y * CELL_SIZE);
    glVertex2i(x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE);
    glVertex2i(x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE + CELL_SIZE);
    glVertex2i(x * CELL_SIZE, y * CELL_SIZE + CELL_SIZE);
    glEnd();
}

// Çizim fonksiyonu
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Yýlaný çiz (yeþil)
    for (int i = 0; i < snakeLength; i++) {
        drawSquare(snake[i].x, snake[i].y, 0.0, 1.0, 0.0);
    }

    // Yemi çiz (kýrmýzý)
    drawSquare(food.x, food.y, 1.0, 0.0, 0.0);

    // Oyun bittiyse mesaj göster
    if (gameOver) {
        printf("Oyun Bitti! Skor: %d\n", score);
        exit(0); // Konsolda mesaj gösterip çýk
    }

    glutSwapBuffers(); // Çift tamponlama için
}

// Yýlaný hareket ettirme
void moveSnake() {
    // Yýlanýn gövdesini kaydýr
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Baþýn yeni pozisyonu
    switch (direction) {
    case 0: snake[0].y++; break; // Yukarý
    case 1: snake[0].x++; break; // Sað
    case 2: snake[0].y--; break; // Aþaðý
    case 3: snake[0].x--; break; // Sol
    }

    // Çarpýþma kontrolü: Duvarlar
    if (snake[0].x < 0 || snake[0].x >= GRID_SIZE || snake[0].y < 0 || snake[0].y >= GRID_SIZE) {
        gameOver = true;
    }

    // Çarpýþma kontrolü: Kendi kuyruðu
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameOver = true;
        }
    }

    // Yem yeme kontrolü
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snakeLength++;
        score += 10;
        // Yeni yem konumu
        food.x = rand() % GRID_SIZE;
        food.y = rand() % GRID_SIZE;
    }
}

// Zamanlayýcý fonksiyonu (oyun döngüsü)
void timer(int value) {
    if (!gameOver) {
        moveSnake();
        glutPostRedisplay(); // Ekraný yenile
        glutTimerFunc(100, timer, 0); // 100ms'de bir güncelle
    }
}

// Klavye giriþi
void keyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP: if (direction != 2) direction = 0; break; // Yukarý, aþaðý gitmiyorsa
    case GLUT_KEY_RIGHT: if (direction != 3) direction = 1; break; // Sað, sol gitmiyorsa
    case GLUT_KEY_DOWN: if (direction != 0) direction = 2; break; // Aþaðý, yukarý gitmiyorsa
    case GLUT_KEY_LEFT: if (direction != 1) direction = 3; break; // Sol, sað gitmiyorsa
    }
}

// Ana fonksiyon
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Çift tamponlama
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE); // 600x600 pencere
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Snake Oyunu");
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(keyboard); // Yön tuþlarý için
    glutTimerFunc(0, timer, 0); // Oyun döngüsü
    glutMainLoop();
    return 0;
}