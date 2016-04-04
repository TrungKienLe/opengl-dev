
import processing.serial.*;

// Cổng UART
Serial myPort;
PFont font;

// Kích thước cửa sổ
final int VIEW_SIZE_X = 1000, VIEW_SIZE_Y = 800;

// Imagage cho các cạnh của hình hộp
PImage topSide, downSide, frontSide, rightSide;

float x_angle = 0;
float y_angle = 0;
float z_angle = 0;

float x_angle_f = 0;
float y_angle_f = 0;
float z_angle_f = 0;


float dt = 0;
float test_value = 0;

//float [] Euler = new float[3];

void setup()
{
  size(VIEW_SIZE_X, VIEW_SIZE_Y, P3D);
  textureMode(NORMAL);
  fill(255);
  stroke(color(44, 48, 32));
  font = loadFont("CourierNew36.vlw"); 

  // Setup UART - In ra list các cổng hiện có, trong trường hợp này mở cổng UART 10, tốc độ 115200
  println(Serial.list());
  myPort = new Serial(this, "COM9", 115200);
  //myPort = new Serial(this, "COM9", 9600);

  // Load các ảnh vào biến tương ứng với các cạnh hình hộp
  // Các ảnh này được lưu ở thư mục data
  topSide   = loadImage("MPU6050 A.png");
  downSide  = loadImage("MPU6050 B.png");
  frontSide = loadImage("MPU6050 E.png");
  rightSide  = loadImage("MPU6050 F.png");


  delay(100);
  myPort.clear();
}

void draw()
{

  background(#000000);
  // Đọc giá trị UART
  serialEvent();

  // Vẽ cảm biến MPU6050
  drawCube() ;

  // Add text
  textFont(font, 25);
  fill(255, 255, 255);
  text("MPU6000 - Raw data\n", 20, 20);

  textFont(font, 20);
  fill(255, 255, 255);
  text("X: " + str(x_angle) + " deg", 40, 60);
  text("Y: " + str(y_angle) + " deg", 40, 100);
  text("Z: " + str(z_angle) + " deg", 40, 140);


  textFont(font, 15);
  text("Design by Trung Kien", 0, VIEW_SIZE_Y - 50);
}

void serialEvent()
{
  String msg = myPort.readStringUntil((int) '\n');
  //delay(10);
  if (msg != null && msg.length() > 0)
  {
    String []input_arr = split(msg, ",");

    x_angle = float(input_arr[0])/100;
    y_angle = float(input_arr[1])/100;
    z_angle = float(input_arr[2])/100;
    

  }
}

void drawCube() 
{  
  pushMatrix();
  translate(VIEW_SIZE_X/2, VIEW_SIZE_Y/2 + 50, 0);
  //scale(5,5,5);
  scale(10);

  // Update góc quay
  // Lưu ý, chiều tọa độ của processing và tọa độ thực tế của acc là khác nhau.
  x_angle_f = (x_angle < 0 ? x_angle + 360 : x_angle);
  y_angle_f = (y_angle < 0 ? y_angle + 360 : y_angle);
  
  rotateZ(radians(x_angle_f));
  rotateX(radians(360 - y_angle_f));
  //rotateY(radians(z_angle));

  //Ve ca khoi cua hinh hop
  topboard(topSide);
  botomboard(downSide);
  sideboarda(frontSide);
  sideboardb(rightSide);

  popMatrix();
}
void topboard(PImage imag)
{
  beginShape(QUADS);
  texture(imag);

  // -Y "top face;
  // Tọa độ của các đỉnh và điểm gán ảnh tương ứng với các đỉnh.
  // Ví dụ như dòng đầu tiên định nghĩa là điểm đầu và ứng với đỉnh đầu tiên của anh load vào
  // Trong chế độ textureMode(NORMAL) giá trị u, v ở cuối được chuẩn hóa về 0..1. 
  // Nếu để 1 thì cùng kích thước. Nhỏ hơn là phóng to, lớn hơn 1 là thu nhỏ.

  vertex(-20, -1, -15, 0, 0);
  vertex( 20, -1, -15, 1, 0);
  vertex( 20, -1, 15, 1, 1);
  vertex(-20, -1, 15, 0, 1);

  endShape();
}

void botomboard(PImage imag) {
  beginShape(QUADS);
  texture(imag);

  // +Y "bottom" face
  vertex(-20, 1, 15, 0, 0);
  vertex( 20, 1, 15, 1, 0);
  vertex( 20, 1, -15, 1, 1);
  vertex(-20, 1, -15, 0, 1);

  endShape();
}


void sideboarda(PImage imag) {
  beginShape(QUADS);
  texture(imag);

  // +Z "front" face
  vertex(-20, -1, 15, 0, 0);
  vertex( 20, -1, 15, 1, 0);
  vertex( 20, 1, 15, 1, 1);
  vertex(-20, 1, 15, 0, 1);

  // -Z "back" face
  vertex( 20, -1, -15, 0, 0);
  vertex(-20, -1, -15, 1, 0);
  vertex(-20, 1, -15, 1, 1);
  vertex( 20, 1, -15, 0, 1);


  endShape();
}

void sideboardb(PImage imag) {
  beginShape(QUADS);
  texture(imag);

  // +X "right" face
  vertex( 20, -1, 15, 0, 0);
  vertex( 20, -1, -15, 1, 0);
  vertex( 20, 1, -15, 1, 1);
  vertex( 20, 1, 15, 0, 1);

  // -X "left" face
  vertex(-20, -1, -15, 0, 0);
  vertex(-20, -1, 15, 1, 0);
  vertex(-20, 1, 15, 1, 1);
  vertex(-20, 1, -15, 0, 1);

  endShape();
}

