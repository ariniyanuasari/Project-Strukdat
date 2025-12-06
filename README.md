# Project-Strukdat
1. Inisialisasi Window, ada pada bagian program ini
InitWindow(800, 600, "Brute Force vs Quadtree Collision");
SetTargetFPS(150);
> Program membuat window berukuran 800×600 dan mengatur target FPS ke 150 agar simulasi berjalan halus
2. Membuat Bola-Bola
std::vector<Ball> balls;
int totalBalls = 200;
bool useQuadtree = true;
> Membuat 200 bola dengan:
posisi acak
kecepatan acak
warna acak
radius tetap (15px)
Semua bola disimpan dalam std::vector<Ball>.
3. Input: Switch Mode
if (IsKeyPressed(KEY_SPACE)) useQuadtree = !useQuadtree;
> Menekan SPACE akan mengganti mode:
Quadtree Mode / Brute Force Mode
4. Update Gerakan Bola
b.pos.x += b.vel.x;
b.pos.y += b.vel.y;
> Program menggerakkan setiap bola sesuai velocity-nya
5. Mode 1: Collision dengan Quadtree
Jika useQuadtree = true:
a. Buat quadtree
Quadtree qt({0,0,800,600}, 6);
>Boundary quadtree adalah seluruh layar.
b. Masukkan semua bola ke quadtree
for (auto &b : balls) qt.insert(&b);
c. Untuk setiap bola:
Rectangle area{
    b.pos.x - b.radius - 1,
    b.pos.y - b.radius - 1,
    b.radius * 2 + 2,
    b.radius * 2 + 2
};
>Program hanya mencari bola lain yang berada di area sekitarnya saja.
>Quadtree kemudian mengembalikan daftar bola di area tersebut:
qt.query(area, found);
d. Cek tabrakan antar bola yang ditemukan
for (int i = 0; i < found.size(); i++)
    for (int j = i+1; j < found.size(); j++)
        HandleCollision(found[i], found[j]);
>Yang dicek hanya bola yang berdekatan → jauh lebih cepat daripada brute force jika bola banyak.
6. Mode 2: Collision dengan Brute Force
> Jika useQuadtree = false:
for (int i = 0; i < totalBalls; i++) {
    for (int j = i + 1; j < totalBalls; j++) {
        HandleCollision(&balls[i], &balls[j]);
    }
}
> Program mengecek semua pasangan bola → O(n²).
Semakin banyak bola, semakin lambat.
7. Drawing Frame
> Setiap frame:
background dibersihkan
bola digambar
teks mode ditampilkan (QUADTREE atau BRUTE FORCE):
DrawText(useQuadtree ? "MODE: QUADTREE..." : "MODE: BRUTE FORCE...", 10, 10, 20, WHITE);
8. Menutup Window
CloseWindow();
> Program selesai.
