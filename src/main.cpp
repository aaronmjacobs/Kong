#include <Kontroller/Kontroller.h>

#include <array>
#include <cmath>
#include <cstdio>
#include <chrono>
#include <random>
#include <thread>

namespace {

constexpr size_t kScreenWidth = 8;
constexpr size_t kScreenHeight = 3;

constexpr size_t kLeft = 1;
constexpr size_t kRight = 7;
constexpr size_t kBottom = 0;
constexpr size_t kTop = 2;

struct Vec2 {
   float x, y;

   Vec2() : x(0.0f), y(0.0f) {}

   Vec2(float x, float y) : x(x), y(y) {}

   void operator+=(const Vec2& other) {
      x += other.x;
      y += other.y;
   }

   void operator*=(const Vec2& other) {
      x *= other.x;
      y *= other.y;
   }

   void operator*=(float val) {
      x *= val;
      y *= val;
   }

   void operator/=(const Vec2& other) {
      x /= other.x;
      y /= other.y;
   }

   void operator/=(float val) {
      x /= val;
      y /= val;
   }
};

Vec2 operator*(const Vec2& vec, float val) {
   return Vec2(vec.x * val, vec.y * val);
}

class PongGame {
private:
   const float initialSpeed;
   Vec2 ballPos;
   Vec2 ballVel;
   int playerOneScore { 0 };
   int playerTwoScore { 0 };
   std::random_device randomDevice;
   std::uniform_real_distribution<float> randomDistribution;

public:
   PongGame(float initialSpeed);

   void reset();

   void tick(float dt, float playerOnePaddle, float playerTwoPaddle);

   void draw(std::array<std::array<bool, 3>, 8>& pixels);
};

PongGame::PongGame(float initialSpeed)
   : initialSpeed(initialSpeed), randomDistribution(1.0f, 2.0f) {
   reset();
}

void PongGame::reset() {
   printf("Player 1: %d, Player 2: %d\n", playerOneScore, playerTwoScore);

   ballPos.x = (static_cast<float>(kLeft) + static_cast<float>(kRight)) / 2.0f;
   ballPos.y = (static_cast<float>(kBottom) + static_cast<float>(kTop)) / 2.0f;

   ballVel.x = randomDistribution(randomDevice);
   ballVel.y = randomDistribution(randomDevice);
   float magnitude = std::sqrt(ballVel.x * ballVel.x + ballVel.y * ballVel.y);
   ballVel /= magnitude;

   ballVel.x *= initialSpeed * (randomDistribution(randomDevice) > 1.5f ? 1.0f : -1.0f);
   ballVel.y *= initialSpeed * (randomDistribution(randomDevice) > 1.5f ? 1.0f : -1.0f);
}

void PongGame::tick(float dt, float playerOnePaddle, float playerTwoPaddle) {
   ballPos += ballVel * dt;

   constexpr float kHalfPaddleSize = 0.75f;

   float xCorrection = 0.0f;
   if (ballPos.x < kLeft) {
      // TODO Back solve for position at boundry

      float paddlePos = (1.0f - playerOnePaddle) * kBottom + playerOnePaddle * kTop;
      if (ballPos.y >= paddlePos - kHalfPaddleSize && ballPos.y <= paddlePos + kHalfPaddleSize) {
         xCorrection = 2.0f * (kLeft - ballPos.x);
      } else {
         ++playerTwoScore;
         reset();
      }
   } else if (ballPos.x > kRight) {
      // TODO Back solve for position at boundry

      float paddlePos = (1.0f - playerTwoPaddle) * kBottom + playerTwoPaddle * kTop;
      if (ballPos.y >= paddlePos - kHalfPaddleSize && ballPos.y <= paddlePos + kHalfPaddleSize) {
         xCorrection = 2.0f * (kRight - ballPos.x);
      } else {
         ++playerOneScore;
         reset();
      }
   }

   float yCorrection = 0.0f;
   if (ballPos.y < kBottom) {
      yCorrection = 2.0f * (kBottom - ballPos.y);
   } else if (ballPos.y > kTop) {
      yCorrection = 2.0f * (kTop - ballPos.y);
   }

   if (xCorrection != 0.0f) {
      ballVel.x *= -1.0f;
      ballPos.x += xCorrection;
   }

   if (yCorrection != 0.0f) {
      ballVel.y *= -1.0f;
      ballPos.y += yCorrection;
   }

   const float xSpeedup = 0.5f;
   const float ySpeedup = 0.01f;

   ballVel.x += xSpeedup * dt * (ballVel.x >= 0.0f ? 1.0f : -1.0f);
   ballVel.y += ySpeedup * dt * (ballVel.y >= 0.0f ? 1.0f : -1.0f);
}

void PongGame::draw(std::array<std::array<bool, kScreenHeight>, kScreenWidth>& pixels) {
   pixels = {};

   int ballX = (int)(ballPos.x + 0.5f);
   int ballY = (int)(ballPos.y + 0.5f);
   if ((ballX >= kLeft && ballX <= kRight) &&
       (ballY >= kBottom && ballY <= kTop)) {
      pixels[ballX][ballY] = true;
   }
}

bool waitForConnection(Kontroller& kontroller) {
   int numAttemptsLeft = 10;
   while (!kontroller.connect() && --numAttemptsLeft > 0) {
      std::printf("Unable to connect to the KORG nanoKontrol2, trying %d more time%s\n", numAttemptsLeft, numAttemptsLeft > 1 ? "s" : "");
      std::this_thread::sleep_for(std::chrono::seconds(1));
   }

   return numAttemptsLeft > 0;
}

void draw(Kontroller& kontroller, const std::array<std::array<bool, kScreenHeight>, kScreenWidth>& pixels) {
   static const std::array<std::array<Kontroller::LED, 3>, 8> LEDs {{
      Kontroller::LED::kCol1R, Kontroller::LED::kCol1M, Kontroller::LED::kCol1S,
      Kontroller::LED::kCol2R, Kontroller::LED::kCol2M, Kontroller::LED::kCol2S,
      Kontroller::LED::kCol3R, Kontroller::LED::kCol3M, Kontroller::LED::kCol3S,
      Kontroller::LED::kCol4R, Kontroller::LED::kCol4M, Kontroller::LED::kCol4S,
      Kontroller::LED::kCol5R, Kontroller::LED::kCol5M, Kontroller::LED::kCol5S,
      Kontroller::LED::kCol6R, Kontroller::LED::kCol6M, Kontroller::LED::kCol6S,
      Kontroller::LED::kCol7R, Kontroller::LED::kCol7M, Kontroller::LED::kCol7S,
      Kontroller::LED::kCol8R, Kontroller::LED::kCol8M, Kontroller::LED::kCol8S,
   }};

   for (size_t x = 0; x < pixels.size(); ++x) {
      for (size_t y = 0; y < pixels[x].size(); ++y) {
         kontroller.setLEDOn(LEDs[x][y], pixels[x][y]);
      }
   }
}

} // namespace

int main(int argc, char* argv[]) {
   Kontroller kontroller;

   if (!waitForConnection(kontroller)) {
      return 0;
   }
   kontroller.enableLEDControl(true);

   PongGame game(3.0f);
   std::array<std::array<bool, kScreenHeight>, kScreenWidth> pixels = {};

   std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
   bool quit = false;
   while (!quit && kontroller.isConnected()) {
      std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
      double dt = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastTime).count();
      lastTime = now;

      kontroller.poll();
      const Kontroller::State& state = kontroller.getState();
      quit = state.stop;

      float leftPaddlePos = state.columns[0].slider;
      float rightPaddlePos = state.columns[7].slider;

      game.tick(static_cast<float>(dt), leftPaddlePos, rightPaddlePos);
      game.draw(pixels);

      draw(kontroller, pixels);

      std::this_thread::sleep_for(std::chrono::milliseconds(16));
   }

   // Clear the LEDs
   if (kontroller.isConnected()) {
      pixels = {};
      draw(kontroller, pixels);
      kontroller.enableLEDControl(false);
   }

   return 0;
}

