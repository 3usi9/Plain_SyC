#ifndef _COMMON_H
#define _COMMON_H
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#define DEBUG
#include<set>
#include<map>
#include<string>
#include<vector>
#include<algorithm>
#include<assert.h>
#define EVAL_COMPILE_MAX_LEVEL 10
#define COLOR_CODE
#ifdef COLOR_CODE
#define COLOR_RED     "\33[1;31m"
#define COLOR_GREEN   "\33[1;32m"
#define COLOR_YELLOW  "\33[1;33m"
#define COLOR_BLUE    "\33[1;34m"
#define COLOR_PURPLE  "\33[1;35m"
#define COLOR_CYAN    "\33[1;36m"
#define COLOR_WHITE   "\33[1;37m"
#define COLOR_NONE    "\33[0m"
#else
#define COLOR_RED     ""
#define COLOR_GREEN   ""
#define COLOR_YELLOW  ""
#define COLOR_BLUE    ""
#define COLOR_PURPLE  ""
#define COLOR_CYAN    ""
#define COLOR_WHITE   ""
#define COLOR_NONE    ""
#endif // COLOR_CODE


enum CONST_STATUS_T { CONST_FALSE = 0, CONST_TRUE, CONST_UNKNOWN };


#define GLOBL_VAR(name) ("$" + (name))
#define LOCAL_VAR(name) ("%" + (name))
#define GLOBL_ARR(name) ("$&" + (name))
#define LOCAL_ARR(name) ("%&" + (name))
#define REFERENCE_ARR(name) ("~" + (name))
#define GLOBL_FNC(name) ("!" + (name))
#define ARG(count) ("@" + (count))

#define IS_ARRAY  true
#define NOT_ARRAY false

#define CHECK_CROSS(x, y, u, v) \
  ((y) >= (u) && (x) <= (v))
#define LIT2STR(x) # x

#define ROUNDUP(x, y) ( \
{ \
  (((x) + ((y) - 1)) / (y)) * (y); \
} \
)

#define ROUNDDOWN(x, y) ( \
{ \
  (x) - ((x) % (y)); \
} \
)

#define COMPILE_WARNING(x)			\
  {						\
    perror(x);					\
    assert(0);					\
  }

#define COMPILE_ERROR(x) \
  {						\
    perror(x);					\
    assert(0);					\
  }

int get_low(int a);

#define IS_POWER_OF_2(x) \
  ((x) == ((x) & (-(x))))

std::set<std::string> mySetUnion(const std::set<std::string>& s1,
		       const std::set<std::string>& s2);

std::set<std::string> mySetDifference(const std::set<std::string>& s1,
			    const std::set<std::string>& s2);

std::set<std::string> mySetIntersect(const std::set<std::string>& s1,
			   const std::set<std::string>& s2);


std::set<int> mySetUnion(const std::set<int>& s1,
		       const std::set<int>& s2);

std::set<int> mySetDifference(const std::set<int>& s1,
			    const std::set<int>& s2);

std::set<int> mySetIntersect(const std::set<int>& s1,
			   const std::set<int>& s2);


  
#define REWRITE_SEARCH_MAX_DEPTH 100
// #define ONLINE_JUDGE
#ifdef ONLINE_JUDGE
#define myAssert(assert_body, retn_code)	\
  {						\
    if(assert_body){}				\
    else exit(retn_code);			\
  }
#else
#define myAssert(assert_body, retn_code)	\
  {						\
    if(assert_body){}				\
    else assert(0);				\
  }

#endif
#define ASSERT_EXIT_CODE_1 1
#define ASSERT_EXIT_CODE_2 2
#define ASSERT_EXIT_CODE_3 3
#define ASSERT_EXIT_CODE_4 4
#define ASSERT_EXIT_CODE_5 5
#define ASSERT_EXIT_CODE_6 6
#define ASSERT_EXIT_CODE_7 7
#define ASSERT_EXIT_CODE_8 8
#define ASSERT_EXIT_CODE_9 9
#define ASSERT_EXIT_CODE_10 10
#define ASSERT_EXIT_CODE_11 11
#define ASSERT_EXIT_CODE_12 12
#define ASSERT_EXIT_CODE_13 13
#define ASSERT_EXIT_CODE_14 14
#define ASSERT_EXIT_CODE_15 15
#define ASSERT_EXIT_CODE_16 16
#define ASSERT_EXIT_CODE_17 17
#define ASSERT_EXIT_CODE_18 18
#define ASSERT_EXIT_CODE_19 19
#define ASSERT_EXIT_CODE_20 20
#define ASSERT_EXIT_CODE_21 21
#define ASSERT_EXIT_CODE_22 22
#define ASSERT_EXIT_CODE_23 23
#define ASSERT_EXIT_CODE_24 24
#define ASSERT_EXIT_CODE_25 25
#define ASSERT_EXIT_CODE_26 26
#define ASSERT_EXIT_CODE_27 27
#define ASSERT_EXIT_CODE_28 28
#define ASSERT_EXIT_CODE_29 29
#define ASSERT_EXIT_CODE_30 30
#define ASSERT_EXIT_CODE_31 31
#define ASSERT_EXIT_CODE_32 32
#define ASSERT_EXIT_CODE_33 33
#define ASSERT_EXIT_CODE_34 34
#define ASSERT_EXIT_CODE_35 35
#define ASSERT_EXIT_CODE_36 36
#define ASSERT_EXIT_CODE_37 37
#define ASSERT_EXIT_CODE_38 38
#define ASSERT_EXIT_CODE_39 39
#define ASSERT_EXIT_CODE_40 40
#define ASSERT_EXIT_CODE_41 41
#define ASSERT_EXIT_CODE_42 42
#define ASSERT_EXIT_CODE_43 43
#define ASSERT_EXIT_CODE_44 44
#define ASSERT_EXIT_CODE_45 45
#define ASSERT_EXIT_CODE_46 46
#define ASSERT_EXIT_CODE_47 47
#define ASSERT_EXIT_CODE_48 48
#define ASSERT_EXIT_CODE_49 49
#define ASSERT_EXIT_CODE_50 50
#define ASSERT_EXIT_CODE_51 51
#define ASSERT_EXIT_CODE_52 52
#define ASSERT_EXIT_CODE_53 53
#define ASSERT_EXIT_CODE_54 54
#define ASSERT_EXIT_CODE_55 55
#define ASSERT_EXIT_CODE_56 56
#define ASSERT_EXIT_CODE_57 57
#define ASSERT_EXIT_CODE_58 58
#define ASSERT_EXIT_CODE_59 59
#define ASSERT_EXIT_CODE_60 60
#define ASSERT_EXIT_CODE_61 61
#define ASSERT_EXIT_CODE_62 62
#define ASSERT_EXIT_CODE_63 63
#define ASSERT_EXIT_CODE_64 64
#define ASSERT_EXIT_CODE_65 65
#define ASSERT_EXIT_CODE_66 66
#define ASSERT_EXIT_CODE_67 67
#define ASSERT_EXIT_CODE_68 68
#define ASSERT_EXIT_CODE_69 69
#define ASSERT_EXIT_CODE_70 70
#define ASSERT_EXIT_CODE_71 71
#define ASSERT_EXIT_CODE_72 72
#define ASSERT_EXIT_CODE_73 73
#define ASSERT_EXIT_CODE_74 74
#define ASSERT_EXIT_CODE_75 75
#define ASSERT_EXIT_CODE_76 76
#define ASSERT_EXIT_CODE_77 77
#define ASSERT_EXIT_CODE_78 78
#define ASSERT_EXIT_CODE_79 79
#define ASSERT_EXIT_CODE_80 80
#define ASSERT_EXIT_CODE_81 81
#define ASSERT_EXIT_CODE_82 82
#define ASSERT_EXIT_CODE_83 83
#define ASSERT_EXIT_CODE_84 84
#define ASSERT_EXIT_CODE_85 85
#define ASSERT_EXIT_CODE_86 86
#define ASSERT_EXIT_CODE_87 87
#define ASSERT_EXIT_CODE_88 88
#define ASSERT_EXIT_CODE_89 89
#define ASSERT_EXIT_CODE_90 90
#define ASSERT_EXIT_CODE_91 91
#define ASSERT_EXIT_CODE_92 92
#define ASSERT_EXIT_CODE_93 93
#define ASSERT_EXIT_CODE_94 94
#define ASSERT_EXIT_CODE_95 95
#define ASSERT_EXIT_CODE_96 96
#define ASSERT_EXIT_CODE_97 97
#define ASSERT_EXIT_CODE_98 98
#define ASSERT_EXIT_CODE_99 99
#define ASSERT_EXIT_CODE_100 100
#define ASSERT_EXIT_CODE_101 101
#define ASSERT_EXIT_CODE_102 102
#define ASSERT_EXIT_CODE_103 103
#define ASSERT_EXIT_CODE_104 104
#define ASSERT_EXIT_CODE_105 105
#define ASSERT_EXIT_CODE_106 106
#define ASSERT_EXIT_CODE_107 107
#define ASSERT_EXIT_CODE_108 108
#define ASSERT_EXIT_CODE_109 109
#define ASSERT_EXIT_CODE_110 110
#define ASSERT_EXIT_CODE_111 111
#define ASSERT_EXIT_CODE_112 112
#define ASSERT_EXIT_CODE_113 113
#define ASSERT_EXIT_CODE_114 114
#define ASSERT_EXIT_CODE_115 115
#define ASSERT_EXIT_CODE_116 116
#define ASSERT_EXIT_CODE_117 117
#define ASSERT_EXIT_CODE_118 118
#define ASSERT_EXIT_CODE_119 119
#define ASSERT_EXIT_CODE_120 120
#define ASSERT_EXIT_CODE_121 121
#define ASSERT_EXIT_CODE_122 122
#define ASSERT_EXIT_CODE_123 123
#define ASSERT_EXIT_CODE_124 124
#define ASSERT_EXIT_CODE_125 125
#define ASSERT_EXIT_CODE_126 126
#define ASSERT_EXIT_CODE_127 127
#define ASSERT_EXIT_CODE_128 128
#define ASSERT_EXIT_CODE_129 129
#define ASSERT_EXIT_CODE_130 130
#define ASSERT_EXIT_CODE_131 131
#define ASSERT_EXIT_CODE_132 132
#define ASSERT_EXIT_CODE_133 133
#define ASSERT_EXIT_CODE_134 134
#define ASSERT_EXIT_CODE_135 135
#define ASSERT_EXIT_CODE_136 136
#define ASSERT_EXIT_CODE_137 137
#define ASSERT_EXIT_CODE_138 138
#define ASSERT_EXIT_CODE_139 139
// [139] is the return value of [Segmentation Fault]
#define ASSERT_EXIT_CODE_140 140
#define ASSERT_EXIT_CODE_141 141
#define ASSERT_EXIT_CODE_142 142
#define ASSERT_EXIT_CODE_143 143
#define ASSERT_EXIT_CODE_144 144
#define ASSERT_EXIT_CODE_145 145
#define ASSERT_EXIT_CODE_146 146
#define ASSERT_EXIT_CODE_147 147
#define ASSERT_EXIT_CODE_148 148
#define ASSERT_EXIT_CODE_149 149
#define ASSERT_EXIT_CODE_150 150
#define ASSERT_EXIT_CODE_151 151
#define ASSERT_EXIT_CODE_152 152
#define ASSERT_EXIT_CODE_153 153
#define ASSERT_EXIT_CODE_154 154
#define ASSERT_EXIT_CODE_155 155
#define ASSERT_EXIT_CODE_156 156
#define ASSERT_EXIT_CODE_157 157
#define ASSERT_EXIT_CODE_158 158
#define ASSERT_EXIT_CODE_159 159
#define ASSERT_EXIT_CODE_160 160
#define ASSERT_EXIT_CODE_161 161
#define ASSERT_EXIT_CODE_162 162
#define ASSERT_EXIT_CODE_163 163
#define ASSERT_EXIT_CODE_164 164
#define ASSERT_EXIT_CODE_165 165
#define ASSERT_EXIT_CODE_166 166
#define ASSERT_EXIT_CODE_167 167
#define ASSERT_EXIT_CODE_168 168
#define ASSERT_EXIT_CODE_169 169
#define ASSERT_EXIT_CODE_170 170
#define ASSERT_EXIT_CODE_171 171
#define ASSERT_EXIT_CODE_172 172
#define ASSERT_EXIT_CODE_173 173
#define ASSERT_EXIT_CODE_174 174
#define ASSERT_EXIT_CODE_175 175
#define ASSERT_EXIT_CODE_176 176
#define ASSERT_EXIT_CODE_177 177
#define ASSERT_EXIT_CODE_178 178
#define ASSERT_EXIT_CODE_179 179
#define ASSERT_EXIT_CODE_180 180
#define ASSERT_EXIT_CODE_181 181
#define ASSERT_EXIT_CODE_182 182
#define ASSERT_EXIT_CODE_183 183
#define ASSERT_EXIT_CODE_184 184
#define ASSERT_EXIT_CODE_185 185
#define ASSERT_EXIT_CODE_186 186
#define ASSERT_EXIT_CODE_187 187
#define ASSERT_EXIT_CODE_188 188
#define ASSERT_EXIT_CODE_189 189
#define ASSERT_EXIT_CODE_190 190
#define ASSERT_EXIT_CODE_191 191
#define ASSERT_EXIT_CODE_192 192
#define ASSERT_EXIT_CODE_193 193
#define ASSERT_EXIT_CODE_194 194
#define ASSERT_EXIT_CODE_195 195
#define ASSERT_EXIT_CODE_196 196
#define ASSERT_EXIT_CODE_197 197
#define ASSERT_EXIT_CODE_198 198
#define ASSERT_EXIT_CODE_199 199
#define ASSERT_EXIT_CODE_200 200
#define ASSERT_EXIT_CODE_201 201
#define ASSERT_EXIT_CODE_202 202
#define ASSERT_EXIT_CODE_203 203
#define ASSERT_EXIT_CODE_204 204
#define ASSERT_EXIT_CODE_205 205
#define ASSERT_EXIT_CODE_206 206
#define ASSERT_EXIT_CODE_207 207
#define ASSERT_EXIT_CODE_208 208
#define ASSERT_EXIT_CODE_209 209
#define ASSERT_EXIT_CODE_210 210
#define ASSERT_EXIT_CODE_211 211
#define ASSERT_EXIT_CODE_212 212
#define ASSERT_EXIT_CODE_213 213
#define ASSERT_EXIT_CODE_214 214
#define ASSERT_EXIT_CODE_215 215
#define ASSERT_EXIT_CODE_216 216
#define ASSERT_EXIT_CODE_217 217
#define ASSERT_EXIT_CODE_218 218
#define ASSERT_EXIT_CODE_219 219
#define ASSERT_EXIT_CODE_220 220
#define ASSERT_EXIT_CODE_221 221
#define ASSERT_EXIT_CODE_222 222
#define ASSERT_EXIT_CODE_223 223
#define ASSERT_EXIT_CODE_224 224
#define ASSERT_EXIT_CODE_225 225
#define ASSERT_EXIT_CODE_226 226
#define ASSERT_EXIT_CODE_227 227
#define ASSERT_EXIT_CODE_228 228
#define ASSERT_EXIT_CODE_229 229
#define ASSERT_EXIT_CODE_230 230
#define ASSERT_EXIT_CODE_231 231
#define ASSERT_EXIT_CODE_232 232
#define ASSERT_EXIT_CODE_233 233
#define ASSERT_EXIT_CODE_234 234
#define ASSERT_EXIT_CODE_235 235
#define ASSERT_EXIT_CODE_236 236
#define ASSERT_EXIT_CODE_237 237
#define ASSERT_EXIT_CODE_238 238
#define ASSERT_EXIT_CODE_239 239
#define ASSERT_EXIT_CODE_240 240
#define ASSERT_EXIT_CODE_241 241
#define ASSERT_EXIT_CODE_242 242
#define ASSERT_EXIT_CODE_243 243
#define ASSERT_EXIT_CODE_244 244
#define ASSERT_EXIT_CODE_245 245
#define ASSERT_EXIT_CODE_246 246
#define ASSERT_EXIT_CODE_247 247
#define ASSERT_EXIT_CODE_248 248
#define ASSERT_EXIT_CODE_249 249
#define ASSERT_EXIT_CODE_250 250
#define ASSERT_EXIT_CODE_251 251
#define ASSERT_EXIT_CODE_252 252
#define ASSERT_EXIT_CODE_253 253
#define ASSERT_EXIT_CODE_254 254
#define ASSERT_EXIT_CODE_255 255
#define ASSERT_EXIT_CODE_256 1
#define ASSERT_EXIT_CODE_257 2
#define ASSERT_EXIT_CODE_258 3
#define ASSERT_EXIT_CODE_259 4
#define ASSERT_EXIT_CODE_260 5
#define ASSERT_EXIT_CODE_261 6
#define ASSERT_EXIT_CODE_262 7
#define ASSERT_EXIT_CODE_263 8
#define ASSERT_EXIT_CODE_264 9
#define ASSERT_EXIT_CODE_265 10
#define ASSERT_EXIT_CODE_266 11
#define ASSERT_EXIT_CODE_267 12
#define ASSERT_EXIT_CODE_268 13
#define ASSERT_EXIT_CODE_269 14
#define ASSERT_EXIT_CODE_270 15
    
#endif // _COMMON_H
