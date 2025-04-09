void FK(void) {
  TestArm();
  TestBase();
  TestHorz();
  TestVert();
  
  HT[0][0] = cos(theta1)*cos(theta4) - sin(theta1)*sin(theta4);
  HT[0][1] = -cos(theta1)*sin(theta4) - sin(theta1)*cos(theta4);
  HT[0][2] = 0;
  HT[0][3] = d3*cos(theta1);
  HT[1][0] = sin(theta1)*sin(theta4) + cos(theta1)*sin(theta4);
  HT[1][1] = -sin(theta4)*sin(theta1) + cos(theta1)*cos(theta4);
  HT[1][2] = 0;
  HT[1][3] = d3*sin(theta1);
  HT[2][0] = 0;
  HT[2][1] = 0;
  HT[2][2] = 1;
  HT[2][3] = d1;
  HT[3][0] = 0;
  HT[3][1] = 0;
  HT[3][2] = 1;
  HT[3][3] = 0;
  
  Serial.print(HT[0][3]);
  Serial.print(" ");
  Serial.print(HT[1][3]);
  Serial.print(" ");
  Serial.println(HT[2][3]);

}
