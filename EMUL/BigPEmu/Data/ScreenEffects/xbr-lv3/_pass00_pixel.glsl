#version 430


//This file has been auto-generated from 'xbr-lv3' by author 'Hyllian'. Original license and copyright:

/*

   Copyright (C) 2011-2015 Hyllian - sergiogdb@gmail.com

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons to whom the Software is furnished to do so, subject to the
   following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

*/

layout(binding = 0, std140) uniform UBO
{
    mat4 MVP;
} global;

struct Push
{
    vec4 SourceSize;
    vec4 OriginalSize;
    vec4 OutputSize;
    uint FrameCount;
    float XBR_Y_WEIGHT;
    float XBR_EQ_THRESHOLD;
    float XBR_EQ_THRESHOLD2;
    float XBR_LV2_COEFFICIENT;
    float corner_type;
};

uniform Push params;

layout(binding = 2) uniform sampler2D Source;

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec4 t1;
layout(location = 2) in vec4 t2;
layout(location = 3) in vec4 t3;
layout(location = 4) in vec4 t4;
layout(location = 5) in vec4 t5;
layout(location = 6) in vec4 t6;
layout(location = 7) in vec4 t7;
layout(location = 0) out vec4 FragColor;

bvec4 and(bvec4 A, bvec4 B)
{
    return bvec4(A.x && B.x, A.y && B.y, A.z && B.z, A.w && B.w);
}

vec4 df(vec4 A, vec4 B)
{
    return vec4(abs(A - B));
}

bvec4 eq(vec4 A, vec4 B)
{
    vec4 param = A;
    vec4 param_1 = B;
    return lessThan(df(param, param_1), vec4(params.XBR_EQ_THRESHOLD));
}

bvec4 or(bvec4 A, bvec4 B)
{
    return bvec4(A.x || B.x, A.y || B.y, A.z || B.z, A.w || B.w);
}

bvec4 eq2(vec4 A, vec4 B)
{
    vec4 param = A;
    vec4 param_1 = B;
    return lessThan(df(param, param_1), vec4(params.XBR_EQ_THRESHOLD2));
}

vec4 weighted_distance(vec4 a, vec4 b, vec4 c, vec4 d, vec4 e, vec4 f, vec4 g, vec4 h)
{
    vec4 param = a;
    vec4 param_1 = b;
    vec4 param_2 = a;
    vec4 param_3 = c;
    vec4 param_4 = d;
    vec4 param_5 = e;
    vec4 param_6 = d;
    vec4 param_7 = f;
    vec4 param_8 = g;
    vec4 param_9 = h;
    return (((df(param, param_1) + df(param_2, param_3)) + df(param_4, param_5)) + df(param_6, param_7)) + (df(param_8, param_9) * 4.0);
}

float c_df(vec3 c1, vec3 c2)
{
    vec3 df_1 = abs(c1 - c2);
    return (df_1.x + df_1.y) + df_1.z;
}

void main()
{
    vec2 fp = fract(vTexCoord * params.SourceSize.xy);
    vec3 A1 = texture(Source, t1.xw).xyz;
    vec3 B1 = texture(Source, t1.yw).xyz;
    vec3 C1 = texture(Source, t1.zw).xyz;
    vec3 A = texture(Source, t2.xw).xyz;
    vec3 B = texture(Source, t2.yw).xyz;
    vec3 C = texture(Source, t2.zw).xyz;
    vec3 D = texture(Source, t3.xw).xyz;
    vec3 E = texture(Source, t3.yw).xyz;
    vec3 F = texture(Source, t3.zw).xyz;
    vec3 G = texture(Source, t4.xw).xyz;
    vec3 H = texture(Source, t4.yw).xyz;
    vec3 I = texture(Source, t4.zw).xyz;
    vec3 G5 = texture(Source, t5.xw).xyz;
    vec3 H5 = texture(Source, t5.yw).xyz;
    vec3 I5 = texture(Source, t5.zw).xyz;
    vec3 A0 = texture(Source, t6.xy).xyz;
    vec3 D0 = texture(Source, t6.xz).xyz;
    vec3 G0 = texture(Source, t6.xw).xyz;
    vec3 C4 = texture(Source, t7.xy).xyz;
    vec3 F4 = texture(Source, t7.xz).xyz;
    vec3 I4 = texture(Source, t7.xw).xyz;
    vec4 b = transpose(mat4x3(vec3(B), vec3(D), vec3(H), vec3(F))) * (vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625) * params.XBR_Y_WEIGHT);
    vec4 c = transpose(mat4x3(vec3(C), vec3(A), vec3(G), vec3(I))) * (vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625) * params.XBR_Y_WEIGHT);
    vec4 e = transpose(mat4x3(vec3(E), vec3(E), vec3(E), vec3(E))) * (vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625) * params.XBR_Y_WEIGHT);
    vec4 d = b.yzwx;
    vec4 f = b.wxyz;
    vec4 g = c.zwxy;
    vec4 h = b.zwxy;
    vec4 i = c.wxyz;
    vec4 i4 = transpose(mat4x3(vec3(I4), vec3(C1), vec3(A0), vec3(G5))) * (vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625) * params.XBR_Y_WEIGHT);
    vec4 i5 = transpose(mat4x3(vec3(I5), vec3(C4), vec3(A1), vec3(G0))) * (vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625) * params.XBR_Y_WEIGHT);
    vec4 h5 = transpose(mat4x3(vec3(H5), vec3(F4), vec3(B1), vec3(D0))) * (vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625) * params.XBR_Y_WEIGHT);
    vec4 f4 = h5.yzwx;
    vec4 c1 = i4.yzwx;
    vec4 g0 = i5.wxyz;
    vec4 b1 = h5.zwxy;
    vec4 d0 = h5.wxyz;
    vec4 Ao = vec4(1.0, -1.0, -1.0, 1.0);
    vec4 Bo = vec4(1.0, 1.0, -1.0, -1.0);
    vec4 Co = vec4(1.5, 0.5, -0.5, 0.5);
    vec4 Ax = vec4(1.0, -1.0, -1.0, 1.0);
    vec4 Bx = vec4(0.5, 2.0, -0.5, -2.0);
    vec4 Cx = vec4(1.0, 1.0, -0.5, 0.0);
    vec4 Ay = vec4(1.0, -1.0, -1.0, 1.0);
    vec4 By = vec4(2.0, 0.5, -2.0, -0.5);
    vec4 Cy = vec4(2.0, 0.0, -1.0, 0.5);
    vec4 Az = vec4(6.0, -2.0, -6.0, 2.0);
    vec4 Bz = vec4(2.0, 6.0, -2.0, -6.0);
    vec4 Cz = vec4(5.0, 3.0, -3.0, -1.0);
    vec4 Aw = vec4(2.0, -6.0, -2.0, 6.0);
    vec4 Bw = vec4(6.0, 2.0, -6.0, -2.0);
    vec4 Cw = vec4(5.0, -1.0, -3.0, 3.0);
    vec4 fx = (Ao * fp.y) + (Bo * fp.x);
    vec4 fx_left = (Ax * fp.y) + (Bx * fp.x);
    vec4 fx_up = (Ay * fp.y) + (By * fp.x);
    vec4 fx3_left = (Az * fp.y) + (Bz * fp.x);
    vec4 fx3_up = (Aw * fp.y) + (Bw * fp.x);
    bvec4 interp_restriction_lv1;
    if (params.corner_type == 1.0)
    {
        bvec4 param = notEqual(e, f);
        bvec4 param_1 = notEqual(e, h);
        interp_restriction_lv1 = and(param, param_1);
    }
    else
    {
        if (params.corner_type == 2.0)
        {
            bvec4 param_2 = notEqual(e, f);
            bvec4 param_3 = notEqual(e, h);
            vec4 param_4 = f;
            vec4 param_5 = b;
            vec4 param_6 = h;
            vec4 param_7 = d;
            bvec4 param_8 = not(eq(param_4, param_5));
            bvec4 param_9 = not(eq(param_6, param_7));
            vec4 param_10 = e;
            vec4 param_11 = i;
            bvec4 param_12 = and(param_8, param_9);
            bvec4 param_13 = eq(param_10, param_11);
            vec4 param_14 = f;
            vec4 param_15 = i4;
            bvec4 param_16 = or(param_12, param_13);
            bvec4 param_17 = not(eq(param_14, param_15));
            vec4 param_18 = h;
            vec4 param_19 = i5;
            bvec4 param_20 = and(param_16, param_17);
            bvec4 param_21 = not(eq(param_18, param_19));
            vec4 param_22 = e;
            vec4 param_23 = g;
            bvec4 param_24 = and(param_20, param_21);
            bvec4 param_25 = eq(param_22, param_23);
            vec4 param_26 = e;
            vec4 param_27 = c;
            bvec4 param_28 = or(param_24, param_25);
            bvec4 param_29 = eq(param_26, param_27);
            bvec4 param_30 = and(param_2, param_3);
            bvec4 param_31 = or(param_28, param_29);
            interp_restriction_lv1 = and(param_30, param_31);
        }
        else
        {
            bvec4 param_32 = notEqual(e, f);
            bvec4 param_33 = notEqual(e, h);
            vec4 param_34 = f;
            vec4 param_35 = b;
            vec4 param_36 = f;
            vec4 param_37 = c;
            bvec4 param_38 = not(eq(param_34, param_35));
            bvec4 param_39 = not(eq(param_36, param_37));
            vec4 param_40 = h;
            vec4 param_41 = d;
            vec4 param_42 = h;
            vec4 param_43 = g;
            bvec4 param_44 = not(eq(param_40, param_41));
            bvec4 param_45 = not(eq(param_42, param_43));
            bvec4 param_46 = and(param_38, param_39);
            bvec4 param_47 = and(param_44, param_45);
            vec4 param_48 = e;
            vec4 param_49 = i;
            vec4 param_50 = f;
            vec4 param_51 = f4;
            vec4 param_52 = f;
            vec4 param_53 = i4;
            bvec4 param_54 = not(eq(param_50, param_51));
            bvec4 param_55 = not(eq(param_52, param_53));
            vec4 param_56 = h;
            vec4 param_57 = h5;
            vec4 param_58 = h;
            vec4 param_59 = i5;
            bvec4 param_60 = not(eq(param_56, param_57));
            bvec4 param_61 = not(eq(param_58, param_59));
            bvec4 param_62 = and(param_54, param_55);
            bvec4 param_63 = and(param_60, param_61);
            bvec4 param_64 = eq(param_48, param_49);
            bvec4 param_65 = or(param_62, param_63);
            vec4 param_66 = e;
            vec4 param_67 = g;
            vec4 param_68 = e;
            vec4 param_69 = c;
            bvec4 param_70 = eq(param_66, param_67);
            bvec4 param_71 = eq(param_68, param_69);
            bvec4 param_72 = and(param_64, param_65);
            bvec4 param_73 = or(param_70, param_71);
            bvec4 param_74 = or(param_46, param_47);
            bvec4 param_75 = or(param_72, param_73);
            bvec4 param_76 = and(param_32, param_33);
            bvec4 param_77 = or(param_74, param_75);
            interp_restriction_lv1 = and(param_76, param_77);
        }
    }
    bvec4 param_78 = notEqual(e, g);
    bvec4 param_79 = notEqual(d, g);
    bvec4 interp_restriction_lv2_left = and(param_78, param_79);
    bvec4 param_80 = notEqual(e, c);
    bvec4 param_81 = notEqual(b, c);
    bvec4 interp_restriction_lv2_up = and(param_80, param_81);
    vec4 param_82 = g;
    vec4 param_83 = g0;
    vec4 param_84 = d0;
    vec4 param_85 = g0;
    bvec4 param_86 = eq2(param_82, param_83);
    bvec4 param_87 = not(eq2(param_84, param_85));
    bvec4 interp_restriction_lv3_left = and(param_86, param_87);
    vec4 param_88 = c;
    vec4 param_89 = c1;
    vec4 param_90 = b1;
    vec4 param_91 = c1;
    bvec4 param_92 = eq2(param_88, param_89);
    bvec4 param_93 = not(eq2(param_90, param_91));
    bvec4 interp_restriction_lv3_up = and(param_92, param_93);
    vec4 fx45 = smoothstep(Co - vec4(0.4000000059604644775390625), Co + vec4(0.4000000059604644775390625), fx);
    vec4 fx30 = smoothstep(Cx - vec4(0.4000000059604644775390625), Cx + vec4(0.4000000059604644775390625), fx_left);
    vec4 fx60 = smoothstep(Cy - vec4(0.4000000059604644775390625), Cy + vec4(0.4000000059604644775390625), fx_up);
    vec4 fx15 = smoothstep(Cz - vec4(0.4000000059604644775390625), Cz + vec4(0.4000000059604644775390625), fx3_left);
    vec4 fx75 = smoothstep(Cw - vec4(0.4000000059604644775390625), Cw + vec4(0.4000000059604644775390625), fx3_up);
    vec4 param_94 = e;
    vec4 param_95 = c;
    vec4 param_96 = g;
    vec4 param_97 = i;
    vec4 param_98 = h5;
    vec4 param_99 = f4;
    vec4 param_100 = h;
    vec4 param_101 = f;
    vec4 param_102 = h;
    vec4 param_103 = d;
    vec4 param_104 = i5;
    vec4 param_105 = f;
    vec4 param_106 = i4;
    vec4 param_107 = b;
    vec4 param_108 = e;
    vec4 param_109 = i;
    bvec4 param_110 = lessThan(weighted_distance(param_94, param_95, param_96, param_97, param_98, param_99, param_100, param_101), weighted_distance(param_102, param_103, param_104, param_105, param_106, param_107, param_108, param_109));
    bvec4 param_111 = interp_restriction_lv1;
    bvec4 edr = and(param_110, param_111);
    vec4 param_112 = f;
    vec4 param_113 = g;
    vec4 param_114 = h;
    vec4 param_115 = c;
    bvec4 param_116 = lessThanEqual(df(param_112, param_113) * params.XBR_LV2_COEFFICIENT, df(param_114, param_115));
    bvec4 param_117 = interp_restriction_lv2_left;
    bvec4 edr_left = and(param_116, param_117);
    vec4 param_118 = f;
    vec4 param_119 = g;
    vec4 param_120 = h;
    vec4 param_121 = c;
    bvec4 param_122 = greaterThanEqual(df(param_118, param_119), df(param_120, param_121) * params.XBR_LV2_COEFFICIENT);
    bvec4 param_123 = interp_restriction_lv2_up;
    bvec4 edr_up = and(param_122, param_123);
    bvec4 edr3_left = interp_restriction_lv3_left;
    bvec4 edr3_up = interp_restriction_lv3_up;
    bvec4 param_124 = edr;
    bvec4 param_125 = notEqual(fx45, vec4(0.0));
    bvec4 nc45 = and(param_124, param_125);
    bvec4 param_126 = edr_left;
    bvec4 param_127 = notEqual(fx30, vec4(0.0));
    bvec4 param_128 = edr;
    bvec4 param_129 = and(param_126, param_127);
    bvec4 nc30 = and(param_128, param_129);
    bvec4 param_130 = edr_up;
    bvec4 param_131 = notEqual(fx60, vec4(0.0));
    bvec4 param_132 = edr;
    bvec4 param_133 = and(param_130, param_131);
    bvec4 nc60 = and(param_132, param_133);
    bvec4 param_134 = edr;
    bvec4 param_135 = edr_left;
    bvec4 param_136 = edr3_left;
    bvec4 param_137 = notEqual(fx15, vec4(0.0));
    bvec4 param_138 = and(param_134, param_135);
    bvec4 param_139 = and(param_136, param_137);
    bvec4 nc15 = and(param_138, param_139);
    bvec4 param_140 = edr;
    bvec4 param_141 = edr_up;
    bvec4 param_142 = edr3_up;
    bvec4 param_143 = notEqual(fx75, vec4(0.0));
    bvec4 param_144 = and(param_140, param_141);
    bvec4 param_145 = and(param_142, param_143);
    bvec4 nc75 = and(param_144, param_145);
    vec4 param_146 = e;
    vec4 param_147 = f;
    vec4 param_148 = e;
    vec4 param_149 = h;
    bvec4 px = lessThanEqual(df(param_146, param_147), df(param_148, param_149));
    bvec4 nc = bvec4((((nc75.x || nc15.x) || nc30.x) || nc60.x) || nc45.x, (((nc75.y || nc15.y) || nc30.y) || nc60.y) || nc45.y, (((nc75.z || nc15.z) || nc30.z) || nc60.z) || nc45.z, (((nc75.w || nc15.w) || nc30.w) || nc60.w) || nc45.w);
    vec4 final45 = vec4(nc45) * fx45;
    vec4 final30 = vec4(nc30) * fx30;
    vec4 final60 = vec4(nc60) * fx60;
    vec4 final15 = vec4(nc15) * fx15;
    vec4 final75 = vec4(nc75) * fx75;
    vec4 maximo = max(max(max(final15, final75), max(final30, final60)), final45);
    vec3 pix1;
    float blend1;
    if (nc.x)
    {
        pix1 = px.x ? F : H;
        blend1 = maximo.x;
    }
    else
    {
        if (nc.y)
        {
            pix1 = px.y ? B : F;
            blend1 = maximo.y;
        }
        else
        {
            if (nc.z)
            {
                pix1 = px.z ? D : B;
                blend1 = maximo.z;
            }
            else
            {
                if (nc.w)
                {
                    pix1 = px.w ? H : D;
                    blend1 = maximo.w;
                }
            }
        }
    }
    vec3 pix2;
    float blend2;
    if (nc.w)
    {
        pix2 = px.w ? H : D;
        blend2 = maximo.w;
    }
    else
    {
        if (nc.z)
        {
            pix2 = px.z ? D : B;
            blend2 = maximo.z;
        }
        else
        {
            if (nc.y)
            {
                pix2 = px.y ? B : F;
                blend2 = maximo.y;
            }
            else
            {
                if (nc.x)
                {
                    pix2 = px.x ? F : H;
                    blend2 = maximo.x;
                }
            }
        }
    }
    vec3 res1 = mix(E, pix1, vec3(blend1));
    vec3 res2 = mix(E, pix2, vec3(blend2));
    vec3 param_150 = E;
    vec3 param_151 = res1;
    vec3 param_152 = E;
    vec3 param_153 = res2;
    vec3 res = mix(res1, res2, vec3(step(c_df(param_150, param_151), c_df(param_152, param_153))));
    FragColor = vec4(res, 1.0);
}

