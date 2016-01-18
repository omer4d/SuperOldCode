void get_axes(VEC3F wp1, VEC3F wp2, VEC3F wp3, VEC3F tp1, VEC3F tp2, VEC3F tp3, VEC3F *A, VEC3F *B, VEC3F *C)
{
 VEC3F wd1, wd2, wd3, td1, td2, td3;
 VEC3F wn = NORMALIZED_VEC3F(vec3f_normal(wp1, wp2, wp3)), tn = vec3f(0.0, 0.0, 1.0);
 tn.z = fabs(tn.z);

 float k;

 wd1 = VEC3F_DIFF(wp1, wp2);
 wd2 = VEC3F_DIFF(wp2, wp3);
 wd3 = wn;
 td1 = VEC3F_DIFF(tp1, tp2);
 td2 = VEC3F_DIFF(tp2, tp3);
 td3 = tn;

 k = 1.0 / (wd1.x * (wd2.y * wd3.z - wd3.y * wd2.z) -
            wd2.x * (wd1.y * wd3.z - wd3.y * wd1.z) -
            wd3.x * (wd2.y * wd1.z - wd1.y * wd2.z));

 A->x =  (wd1.y * (wd2.z * td3.x - wd3.z * td2.x) +
          wd1.z * (wd3.y * td2.x - wd2.y * td3.x) +
          td1.x * (wd2.y * wd3.z - wd3.y * wd2.z)) * k;
 A->y = -(wd2.x * (wd3.z * td1.x - wd1.z * td3.x) +
          wd2.z * (wd1.x * td3.x - wd3.x * td1.x) +
          td2.x * (wd1.z * wd3.x - wd1.x * wd3.z)) * k;
 A->z =  (wd3.y * (wd2.x * td1.x - wd1.x * td2.x) +
          wd2.y * (wd1.x * td3.x - wd3.x * td1.x) -
          wd1.y * (wd2.x * td3.x - wd3.x * td2.x)) * k;

 B->x =  (wd1.y * (wd2.z * td3.y - wd3.z * td2.y) +
          wd1.z * (wd3.y * td2.y - wd2.y * td3.y) +
          td1.y * (wd2.y * wd3.z - wd3.y * wd2.z)) * k;
 B->y = -(wd2.x * (wd3.z * td1.y - wd1.z * td3.y) +
          wd2.z * (wd1.x * td3.y - wd3.x * td1.y) +
          td2.y * (wd1.z * wd3.x - wd1.x * wd3.z)) * k;
 B->z =  (wd3.y * (wd2.x * td1.y - wd1.x * td2.y) +
          wd2.y * (wd1.x * td3.y - wd3.x * td1.y) -
          wd1.y * (wd2.x * td3.y - wd3.x * td2.y)) * k;

 C->x =  (wd1.y * (wd2.z * td3.z - wd3.z * td2.z) +
          wd1.z * (wd3.y * td2.z - wd2.y * td3.z) +
          td1.z * (wd2.y * wd3.z - wd3.y * wd2.z)) * k;
 C->y = -(wd2.x * (wd3.z * td1.z - wd1.z * td3.z) +
          wd2.z * (wd1.x * td3.z - wd3.x * td1.z) +
          td2.z * (wd1.z * wd3.x - wd1.x * wd3.z)) * k;
 C->z =  (wd3.y * (wd2.x * td1.z - wd1.x * td2.z) +
          wd2.y * (wd1.x * td3.z - wd3.x * td1.z) -
          wd1.y * (wd2.x * td3.z - wd3.x * td2.z)) * k;
}

void transform_to_space(VEC3F A, VEC3F B, VEC3F C, VEC3F wp1, VEC3F tp1, VEC3F wp, VEC3F *tp)
{
 VEC3F v = VEC3F_DIFF(wp, wp1);
 tp->x = tp1.x + VEC3F_DOT_PRODUCT(v, A);
 tp->y = tp1.y + VEC3F_DOT_PRODUCT(v, B);
 tp->z = tp1.z + VEC3F_DOT_PRODUCT(v, C);
}
