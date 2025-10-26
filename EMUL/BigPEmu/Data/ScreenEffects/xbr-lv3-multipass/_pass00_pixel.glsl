#version 430


//This file has been auto-generated from 'xbr-lv3-pass0' by author 'Hyllian'. Original license and copyright:

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
};

uniform Push params;

layout(binding = 2) uniform sampler2D Source;

layout(location = 1) in vec4 t1;
layout(location = 2) in vec4 t2;
layout(location = 3) in vec4 t3;
layout(location = 4) in vec4 t4;
layout(location = 5) in vec4 t5;
layout(location = 6) in vec4 t6;
layout(location = 7) in vec4 t7;
layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 vTexCoord;

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
    return lessThan(df(param, param_1), vec4(15.0));
}

bvec4 or(bvec4 A, bvec4 B)
{
    return bvec4(A.x || B.x, A.y || B.y, A.z || B.z, A.w || B.w);
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

vec4 remapTo01(vec4 v, vec4 high)
{
    return v / high;
}

void main()
{
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
    vec4 b = vec3(14.35200023651123046875, 28.1760005950927734375, 5.4720001220703125) * mat4x3(vec3(B), vec3(D), vec3(H), vec3(F));
    vec4 c = vec3(14.35200023651123046875, 28.1760005950927734375, 5.4720001220703125) * mat4x3(vec3(C), vec3(A), vec3(G), vec3(I));
    vec4 e = vec3(14.35200023651123046875, 28.1760005950927734375, 5.4720001220703125) * mat4x3(vec3(E), vec3(E), vec3(E), vec3(E));
    vec4 d = b.yzwx;
    vec4 f = b.wxyz;
    vec4 g = c.zwxy;
    vec4 h = b.zwxy;
    vec4 i = c.wxyz;
    vec4 i4 = vec3(14.35200023651123046875, 28.1760005950927734375, 5.4720001220703125) * mat4x3(vec3(I4), vec3(C1), vec3(A0), vec3(G5));
    vec4 i5 = vec3(14.35200023651123046875, 28.1760005950927734375, 5.4720001220703125) * mat4x3(vec3(I5), vec3(C4), vec3(A1), vec3(G0));
    vec4 h5 = vec3(14.35200023651123046875, 28.1760005950927734375, 5.4720001220703125) * mat4x3(vec3(H5), vec3(F4), vec3(B1), vec3(D0));
    vec4 f4 = h5.yzwx;
    vec4 c1 = i4.yzwx;
    vec4 g0 = i5.wxyz;
    vec4 b1 = h5.zwxy;
    vec4 d0 = h5.wxyz;
    bvec4 param = notEqual(e, f);
    bvec4 param_1 = notEqual(e, h);
    bvec4 interp_restriction_lv0 = and(param, param_1);
    vec4 param_2 = h;
    vec4 param_3 = h5;
    vec4 param_4 = h;
    vec4 param_5 = i5;
    bvec4 param_6 = not(eq(param_2, param_3));
    bvec4 param_7 = not(eq(param_4, param_5));
    bvec4 comp1 = and(param_6, param_7);
    vec4 param_8 = h;
    vec4 param_9 = d;
    vec4 param_10 = h;
    vec4 param_11 = g;
    bvec4 param_12 = not(eq(param_8, param_9));
    bvec4 param_13 = not(eq(param_10, param_11));
    bvec4 comp2 = and(param_12, param_13);
    vec4 param_14 = f;
    vec4 param_15 = f4;
    vec4 param_16 = f;
    vec4 param_17 = i4;
    bvec4 param_18 = not(eq(param_14, param_15));
    bvec4 param_19 = not(eq(param_16, param_17));
    bvec4 comp3 = and(param_18, param_19);
    vec4 param_20 = f;
    vec4 param_21 = b;
    vec4 param_22 = f;
    vec4 param_23 = c;
    bvec4 param_24 = not(eq(param_20, param_21));
    bvec4 param_25 = not(eq(param_22, param_23));
    bvec4 comp4 = and(param_24, param_25);
    vec4 param_26 = e;
    vec4 param_27 = i;
    bvec4 param_28 = comp3;
    bvec4 param_29 = comp1;
    bvec4 param_30 = eq(param_26, param_27);
    bvec4 param_31 = or(param_28, param_29);
    bvec4 comp5 = and(param_30, param_31);
    vec4 param_32 = e;
    vec4 param_33 = g;
    vec4 param_34 = e;
    vec4 param_35 = c;
    bvec4 param_36 = eq(param_32, param_33);
    bvec4 param_37 = eq(param_34, param_35);
    bvec4 param_38 = comp5;
    bvec4 param_39 = or(param_36, param_37);
    bvec4 param_40 = comp2;
    bvec4 param_41 = or(param_38, param_39);
    bvec4 param_42 = comp4;
    bvec4 param_43 = or(param_40, param_41);
    bvec4 interp_restriction_lv1 = or(param_42, param_43);
    bvec4 param_44 = notEqual(e, g);
    bvec4 param_45 = notEqual(d, g);
    bvec4 interp_restriction_lv2_left = and(param_44, param_45);
    bvec4 param_46 = notEqual(e, c);
    bvec4 param_47 = notEqual(b, c);
    bvec4 interp_restriction_lv2_up = and(param_46, param_47);
    bvec4 param_48 = notEqual(e, g0);
    bvec4 param_49 = notEqual(d0, g0);
    bvec4 interp_restriction_lv3_left = and(param_48, param_49);
    bvec4 param_50 = notEqual(e, c1);
    bvec4 param_51 = notEqual(b1, c1);
    bvec4 interp_restriction_lv3_up = and(param_50, param_51);
    vec4 param_52 = e;
    vec4 param_53 = c;
    vec4 param_54 = g;
    vec4 param_55 = i;
    vec4 param_56 = h5;
    vec4 param_57 = f4;
    vec4 param_58 = h;
    vec4 param_59 = f;
    vec4 param_60 = h;
    vec4 param_61 = d;
    vec4 param_62 = i5;
    vec4 param_63 = f;
    vec4 param_64 = i4;
    vec4 param_65 = b;
    vec4 param_66 = e;
    vec4 param_67 = i;
    bvec4 param_68 = lessThan(weighted_distance(param_52, param_53, param_54, param_55, param_56, param_57, param_58, param_59), weighted_distance(param_60, param_61, param_62, param_63, param_64, param_65, param_66, param_67));
    bvec4 param_69 = interp_restriction_lv0;
    bvec4 edr0 = and(param_68, param_69);
    bvec4 param_70 = edr0;
    bvec4 param_71 = interp_restriction_lv1;
    bvec4 edr = and(param_70, param_71);
    vec4 param_72 = f;
    vec4 param_73 = g;
    vec4 param_74 = h;
    vec4 param_75 = c;
    bvec4 param_76 = interp_restriction_lv2_left;
    bvec4 param_77 = edr;
    bvec4 param_78 = lessThanEqual(df(param_72, param_73) * 2.0, df(param_74, param_75));
    bvec4 param_79 = and(param_76, param_77);
    bvec4 edr_left = and(param_78, param_79);
    vec4 param_80 = f;
    vec4 param_81 = g;
    vec4 param_82 = h;
    vec4 param_83 = c;
    bvec4 param_84 = interp_restriction_lv2_up;
    bvec4 param_85 = edr;
    bvec4 param_86 = greaterThanEqual(df(param_80, param_81), df(param_82, param_83) * 2.0);
    bvec4 param_87 = and(param_84, param_85);
    bvec4 edr_up = and(param_86, param_87);
    vec4 param_88 = f;
    vec4 param_89 = g0;
    vec4 param_90 = h;
    vec4 param_91 = c1;
    bvec4 param_92 = interp_restriction_lv3_left;
    bvec4 param_93 = edr_left;
    bvec4 param_94 = lessThanEqual(df(param_88, param_89) * 4.0, df(param_90, param_91));
    bvec4 param_95 = and(param_92, param_93);
    bvec4 edr3_left = and(param_94, param_95);
    vec4 param_96 = f;
    vec4 param_97 = g0;
    vec4 param_98 = h;
    vec4 param_99 = c1;
    bvec4 param_100 = interp_restriction_lv3_up;
    bvec4 param_101 = edr_up;
    bvec4 param_102 = greaterThanEqual(df(param_96, param_97), df(param_98, param_99) * 4.0);
    bvec4 param_103 = and(param_100, param_101);
    bvec4 edr3_up = and(param_102, param_103);
    vec4 param_104 = e;
    vec4 param_105 = f;
    vec4 param_106 = e;
    vec4 param_107 = h;
    bvec4 px = lessThanEqual(df(param_104, param_105), df(param_106, param_107));
    bvec4 lin3 = bvec4(true);
    bvec4 lin2 = bvec4(true);
    bvec4 lin1 = bvec4(true);
    bvec4 lin0 = bvec4(true);
    bool _783;
    if (edr_left.x)
    {
        _783 = !edr_up.x;
    }
    else
    {
        _783 = edr_left.x;
    }
    bvec2 px0;
    bvec2 px3;
    bvec2 px1;
    if (_783)
    {
        px0 = bvec2(false, px.x);
        px3 = bvec2(px.x, true);
        if (edr3_left.x)
        {
            lin0 = bvec4(false, true, false, false);
            lin3 = bvec4(true, false, false, false);
        }
        else
        {
            lin0 = bvec4(false, false, true, false);
            lin3 = bvec4(false, true, true, false);
        }
    }
    else
    {
        bool _814;
        if (edr_up.x)
        {
            _814 = !edr_left.x;
        }
        else
        {
            _814 = edr_up.x;
        }
        if (_814)
        {
            px0 = bvec2(false, px.x);
            px1 = bvec2(!px.x, false);
            if (edr3_up.x)
            {
                lin0 = bvec4(false, true, false, true);
                lin1 = bvec4(true, false, false, true);
            }
            else
            {
                lin0 = bvec4(false, false, true, true);
                lin1 = bvec4(false, true, true, true);
            }
        }
        else
        {
            if (edr.x)
            {
                edr3_up.x = false;
                edr3_left.x = false;
                edr_up.x = false;
                edr_left.x = false;
                px0 = bvec2(false, px.x);
                lin0 = bvec4(false, false, false, true);
            }
            else
            {
                if (edr0.x)
                {
                    edr3_up.x = false;
                    edr3_left.x = false;
                    edr_up.x = false;
                    edr_left.x = false;
                    px0 = bvec2(false, px.x);
                    lin0 = bvec4(false);
                }
            }
        }
    }
    bool _867;
    if (edr_left.y)
    {
        _867 = !edr_up.y;
    }
    else
    {
        _867 = edr_left.y;
    }
    bvec2 px2;
    if (_867)
    {
        px1 = bvec2(false, px.y);
        px0 = bvec2(px.y, true);
        if (edr3_left.y)
        {
            lin1 = bvec4(false, true, false, false);
            lin0 = bvec4(true, false, false, false);
        }
        else
        {
            lin1 = bvec4(false, false, true, false);
            lin0 = bvec4(false, true, true, false);
        }
    }
    else
    {
        bool _889;
        if (edr_up.y)
        {
            _889 = !edr_left.y;
        }
        else
        {
            _889 = edr_up.y;
        }
        if (_889)
        {
            px1 = bvec2(false, px.y);
            px2 = bvec2(!px.y, false);
            if (edr3_up.y)
            {
                lin1 = bvec4(false, true, false, true);
                lin2 = bvec4(true, false, false, true);
            }
            else
            {
                lin1 = bvec4(false, false, true, true);
                lin2 = bvec4(false, true, true, true);
            }
        }
        else
        {
            if (edr.y)
            {
                edr3_up.y = false;
                edr3_left.y = false;
                edr_up.y = false;
                edr_left.y = false;
                px1 = bvec2(false, px.y);
                lin1 = bvec4(false, false, false, true);
            }
            else
            {
                if (edr0.y)
                {
                    edr3_up.y = false;
                    edr3_left.y = false;
                    edr_up.y = false;
                    edr_left.y = false;
                    px1 = bvec2(false, px.y);
                    lin1 = bvec4(false);
                }
            }
        }
    }
    bool _936;
    if (edr_left.z)
    {
        _936 = !edr_up.z;
    }
    else
    {
        _936 = edr_left.z;
    }
    if (_936)
    {
        px2 = bvec2(false, px.z);
        px1 = bvec2(px.z, true);
        if (edr3_left.z)
        {
            lin2 = bvec4(false, true, false, false);
            lin1 = bvec4(true, false, false, false);
        }
        else
        {
            lin2 = bvec4(false, false, true, false);
            lin1 = bvec4(false, true, true, false);
        }
    }
    else
    {
        bool _958;
        if (edr_up.z)
        {
            _958 = !edr_left.z;
        }
        else
        {
            _958 = edr_up.z;
        }
        if (_958)
        {
            px2 = bvec2(false, px.z);
            px3 = bvec2(!px.z, false);
            if (edr3_up.z)
            {
                lin2 = bvec4(false, true, false, true);
                lin3 = bvec4(true, false, false, true);
            }
            else
            {
                lin2 = bvec4(false, false, true, true);
                lin3 = bvec4(false, true, true, true);
            }
        }
        else
        {
            if (edr.z)
            {
                edr3_up.z = false;
                edr3_left.z = false;
                edr_up.z = false;
                edr_left.z = false;
                px2 = bvec2(false, px.z);
                lin2 = bvec4(false, false, false, true);
            }
            else
            {
                if (edr0.z)
                {
                    edr3_up.z = false;
                    edr3_left.z = false;
                    edr_up.z = false;
                    edr_left.z = false;
                    px2 = bvec2(false, px.z);
                    lin2 = bvec4(false);
                }
            }
        }
    }
    bool _1004;
    if (edr_left.w)
    {
        _1004 = !edr_up.w;
    }
    else
    {
        _1004 = edr_left.w;
    }
    if (_1004)
    {
        px3 = bvec2(false, px.w);
        px2 = bvec2(px.w, true);
        if (edr3_left.w)
        {
            lin3 = bvec4(false, true, false, false);
            lin2 = bvec4(true, false, false, false);
        }
        else
        {
            lin3 = bvec4(false, false, true, false);
            lin2 = bvec4(false, true, true, false);
        }
    }
    else
    {
        bool _1026;
        if (edr_up.w)
        {
            _1026 = !edr_left.w;
        }
        else
        {
            _1026 = edr_up.w;
        }
        if (_1026)
        {
            px3 = bvec2(false, px.w);
            px0 = bvec2(!px.w, false);
            if (edr3_up.w)
            {
                lin3 = bvec4(false, true, false, true);
                lin0 = bvec4(true, false, false, true);
            }
            else
            {
                lin3 = bvec4(false, false, true, true);
                lin0 = bvec4(false, true, true, true);
            }
        }
        else
        {
            if (edr.w)
            {
                edr3_up.w = false;
                edr3_left.w = false;
                edr_up.w = false;
                edr_left.w = false;
                px3 = bvec2(false, px.w);
                lin3 = bvec4(false, false, false, true);
            }
            else
            {
                if (edr0.w)
                {
                    edr3_up.w = false;
                    edr3_left.w = false;
                    edr_up.w = false;
                    edr_left.w = false;
                    px3 = bvec2(false, px.w);
                    lin3 = bvec4(false);
                }
            }
        }
    }
    vec4 info = mat4(vec4(vec4(edr3_left)), vec4(vec4(edr3_up)), vec4(float(px0.x), float(px1.x), float(px2.x), float(px3.x)), vec4(float(px0.y), float(px1.y), float(px2.y), float(px3.y))) * vec4(1.0, 2.0, 4.0, 8.0);
    info += (mat4(vec4(float(lin0.x), float(lin1.x), float(lin2.x), float(lin3.x)), vec4(float(lin0.y), float(lin1.y), float(lin2.y), float(lin3.y)), vec4(float(lin0.z), float(lin1.z), float(lin2.z), float(lin3.z)), vec4(float(lin0.w), float(lin1.w), float(lin2.w), float(lin3.w))) * vec4(16.0, 32.0, 64.0, 128.0));
    vec4 param_108 = info;
    vec4 param_109 = vec4(255.0);
    FragColor = vec4(remapTo01(param_108, param_109));
}

