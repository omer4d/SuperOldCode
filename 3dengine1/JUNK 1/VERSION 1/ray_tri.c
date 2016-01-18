int ray_triangle_x(VEC3F p, VEC3F d, VEC3F v0, VEC3F v1, VEC3F v2)
{
 VEC3F e1, e2, h, s, q;
 float a, f, u, v;

 e1 = VEC3F_DIFF(v1, v0);
 e2 = VEC3F_DIFF(v2, v0);
 h = vec3f_cross(d, e2);
 a = VEC3F_DOT_PRODUCT(e1, h);

 if(a > -0.00001 && a < 0.00001)
  return 0;

 f = 1.0 / a;
 s = VEC3F_DIFF(s, p);
 u = f * VEC3F_DOT_PRODUCT(s, h);

 if (u < 0.0 || u > 1.0)
  return 0;

 q = vec3f_cross(s, e1);
 v = f * VEC3F_DOT_PRODUCT(d, q);

 if(v < 0.0 || u + v > 1.0)
  return 0;

 t = f * VEC3F_DOT_PRODUCT(e2, q);
 if(t > 0.00001)
  return 1;

 return 0;
}
