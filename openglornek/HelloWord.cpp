#include <GL/glut.h>     // OpenGL + GLUT: pencere, çizim, klavye, timer
#include <stdlib.h>      // rand(), srand(), exit() fonksiyonları
#include <stdio.h>       // printf() ile konsola yazı yazmak için
#include <time.h>        // time() fonksiyonu (rastgelelik için)
#include <stdbool.h>     // bool, true, false veri tipleri

// -------------------- OYUN ALANI SABİTLERİ --------------------

#define GRID_SIZE 30     // Oyun alanı 30x30 kareden oluşur
#define CELL_SIZE 20     // Her karenin ekrandaki boyutu 20 piksel
#define WINDOW_SIZE (GRID_SIZE * CELL_SIZE) // Pencere boyutu: 600x600

// -------------------- YAPI TANIMI --------------------

// Yılan ve yemin konumunu tutmak için 2 boyutlu nokta yapısı
typedef struct {
    int x;              // X ekseni (sağ-sol)
    int y;              // Y ekseni (yukarı-aşağı)
} Point;

// -------------------- GLOBAL DEĞİŞKENLER --------------------

Point snake[100];       // Yılanın gövdesi (maksimum 100 parça)
int snakeLength = 3;    // Yılanın başlangıç uzunluğu
Point food;             // Yem nesnesinin konumu

int direction = 1;      // Yılanın yönü: 0=yukarı, 1=sağ, 2=aşağı, 3=sol
bool gameOver = false;  // Oyun bitti mi kontrolü
int score = 0;          // Oyuncunun skoru
int speed = 100;      // Yılanın başlangıç hızı (ms)
int minSpeed = 40;    // Daha fazla hızlanmaması için alt sınır


// -------------------- BAŞLANGIÇ AYARLARI --------------------

void init(void) {

    glClearColor(0.0, 0.0, 0.0, 0.0);  // Arka plan rengini siyah yap

    glMatrixMode(GL_PROJECTION);       // 2D/3D görüntü ayarları
    gluOrtho2D(0, WINDOW_SIZE, 0, WINDOW_SIZE); // 2D koordinat sistemi (600x600)

    srand(time(NULL));                 // Rastgele sayı üretimi için seed

    // Yılanın başlangıç konumu (ekranın ortası)
    snake[0].x = 15; snake[0].y = 15;  // Yılanın başı
    snake[1].x = 14; snake[1].y = 15;  // Gövde
    snake[2].x = 13; snake[2].y = 15;  // Kuyruk

    // İlk yem rastgele bir konuma yerleştirilir
    food.x = rand() % GRID_SIZE;
    food.y = rand() % GRID_SIZE;
}

// -------------------- KARE ÇİZME FONKSİYONU --------------------

void drawSquare(int x, int y, float r, float g, float b) {

    glColor3f(r, g, b);                // Çizim rengi ayarlanır (RGB)

    glBegin(GL_QUADS);                 // Dört köşeli şekil çizimine başlanır

    glVertex2i(x * CELL_SIZE, y * CELL_SIZE);                         // Sol-alt köşe
    glVertex2i(x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE);             // Sağ-alt köşe
    glVertex2i(x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE + CELL_SIZE); // Sağ-üst köşe
    glVertex2i(x * CELL_SIZE, y * CELL_SIZE + CELL_SIZE);             // Sol-üst köşe

    glEnd();                           // Kare çizimi biter
}

// -------------------- EKRAN ÇİZİMİ --------------------

void display(void) {

    glClear(GL_COLOR_BUFFER_BIT);      // Önce ekran temizlenir

    // Yılan yeşil renkte çizilir
    for (int i = 0; i < snakeLength; i++) {
        drawSquare(snake[i].x, snake[i].y, 0.0, 1.0, 0.0);
    }

    // Yem kırmızı renkte çizilir
    drawSquare(food.x, food.y, 1.0, 0.0, 0.0);

    // Oyun bittiyse konsola mesaj yaz ve çık
    if (gameOver) {
        printf("Oyun Bitti! Skor: %d\n", score); // Skoru yazdır
        exit(0);                                 // Pencereyi kapat
    }

    glutSwapBuffers();                // Çift tamponlama (titreme önlenir)
}

// -------------------- YILAN HAREKETİ --------------------

void moveSnake() {

    // Yılanın gövdesi arkadan öne doğru kaydırılır
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Yön bilgisine göre yılanın başı hareket eder
    switch (direction) {
    case 0: snake[0].y++; break;      // Yukarı hareket
    case 1: snake[0].x++; break;      // Sağa hareket
    case 2: snake[0].y--; break;      // Aşağı hareket
    case 3: snake[0].x--; break;      // Sola hareket
    }

    // Duvara çarpma kontrolü
    if (snake[0].x < 0 || snake[0].x >= GRID_SIZE ||
        snake[0].y < 0 || snake[0].y >= GRID_SIZE) {
        gameOver = true;              // Duvara çarptıysa oyun biter
    }

    // Yılanın kendi gövdesine çarpma kontrolü
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x &&
            snake[0].y == snake[i].y) {
            gameOver = true;          // Kendine çarptıysa oyun biter
        }
    }

    // Yem yeme kontrolü
    if (snake[0].x == food.x && snake[0].y == food.y) {

        snakeLength++;                // Yılan uzar
        score += 10;                  // Skor artar

        if (score >= 50 && speed > minSpeed) {
            speed -= 10;        // Hızı artır (timer süresi düşer)
        }
        // Yeni yem rastgele bir konuma yerleştirilir
        food.x = rand() % GRID_SIZE;
        food.y = rand() % GRID_SIZE;
    }
}

// -------------------- TIMER (OYUN DÖNGÜSÜ) --------------------

void timer(int value) {

    if (!gameOver) {                  // Oyun devam ediyorsa
        moveSnake();                  // Yılanı hareket ettir
        glutPostRedisplay();          // Ekranı yenile
        glutTimerFunc(100, timer, 0); // 100 ms sonra tekrar çağır
    }
}

// -------------------- KLAVYE KONTROLÜ --------------------

void keyboard(int key, int x, int y) {

    switch (key) {
    case GLUT_KEY_UP:
        if (direction != 2) direction = 0; // Yukarı (ters yön engellenir)
        break;
    case GLUT_KEY_RIGHT:
        if (direction != 3) direction = 1; // Sağ
        break;
    case GLUT_KEY_DOWN:
        if (direction != 0) direction = 2; // Aşağı
        break;
    case GLUT_KEY_LEFT:
        if (direction != 1) direction = 3; // Sol
        break;
    }
}

// -------------------- ANA FONKSİYON --------------------

int main(int argc, char** argv) {

    glutInit(&argc, argv);                                // GLUT başlatılır
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);          // Çift tamponlama
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);         // 600x600 pencere
    glutInitWindowPosition(100, 100);                     // Pencere konumu
    glutCreateWindow("Snake Oyunu");                      // Pencere başlığı

    init();                                               // Başlangıç ayarları

    glutDisplayFunc(display);                             // Çizim fonksiyonu
    glutSpecialFunc(keyboard);                            // Ok tuşları
    glutTimerFunc(0, timer, 0);                           // Oyun döngüsü

    glutMainLoop();                                       // Sonsuz döngü
    return 0;                                             // Program sonu
}
