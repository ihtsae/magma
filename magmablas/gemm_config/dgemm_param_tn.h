#ifndef DGEMM_PARAM_TN_H 
#define DGEMM_PARAM_TN_H 

#ifdef PRECISION_d
//index, DIM_X, DIM_Y, BLK_M, BLK_N, BLK_K, dim_vec, DIM_XA, DIM_YA, DIM_XB, DIM_YB
#define TN_V_0 2, 16, 16, 32, 2, 1, 2, 16, 2, 16
#define TN_V_1 2, 16, 16, 32, 4, 1, 2, 16, 2, 16
#define TN_V_2 2, 16, 16, 32, 6, 1, 2, 16, 2, 16
#define TN_V_3 2, 16, 16, 32, 8, 1, 2, 16, 2, 16
#define TN_V_4 2, 16, 16, 48, 2, 1, 2, 16, 2, 16
#define TN_V_5 2, 16, 16, 48, 4, 1, 2, 16, 2, 16
#define TN_V_6 2, 16, 16, 48, 6, 1, 2, 16, 2, 16
#define TN_V_7 4, 8, 8, 24, 4, 1, 4, 8, 4, 8
#define TN_V_8 4, 8, 8, 24, 8, 1, 4, 8, 4, 8
#define TN_V_9 4, 8, 8, 24, 12, 1, 4, 8, 4, 8
#define TN_V_10 4, 8, 8, 32, 4, 1, 4, 8, 4, 8
#define TN_V_11 4, 8, 8, 32, 8, 1, 4, 8, 4, 8
#define TN_V_12 4, 8, 8, 40, 4, 1, 4, 8, 4, 8
#define TN_V_13 4, 8, 8, 40, 8, 1, 4, 8, 4, 8
#define TN_V_14 4, 8, 8, 48, 4, 1, 4, 8, 4, 8
#define TN_V_15 4, 8, 8, 56, 4, 1, 4, 8, 4, 8
#define TN_V_16 4, 8, 8, 64, 4, 1, 4, 8, 4, 8
#define TN_V_17 4, 8, 16, 16, 4, 1, 4, 8, 4, 8
#define TN_V_18 4, 8, 16, 16, 8, 1, 4, 8, 4, 8
#define TN_V_19 4, 8, 16, 16, 12, 1, 4, 8, 4, 8
#define TN_V_20 4, 8, 16, 24, 4, 1, 4, 8, 4, 8
#define TN_V_21 4, 8, 16, 24, 8, 1, 4, 8, 4, 8
#define TN_V_22 4, 8, 16, 32, 4, 1, 4, 8, 4, 8
#define TN_V_23 4, 8, 16, 32, 8, 1, 4, 8, 4, 8
#define TN_V_24 4, 8, 16, 40, 4, 1, 4, 8, 4, 8
#define TN_V_25 4, 8, 16, 48, 4, 1, 4, 8, 4, 8
#define TN_V_26 4, 8, 16, 56, 4, 1, 4, 8, 4, 8
#define TN_V_27 4, 8, 24, 16, 4, 1, 4, 8, 4, 8
#define TN_V_28 4, 8, 24, 16, 8, 1, 4, 8, 4, 8
#define TN_V_29 4, 8, 24, 24, 4, 1, 4, 8, 4, 8
#define TN_V_30 4, 8, 24, 24, 8, 1, 4, 8, 4, 8
#define TN_V_31 4, 8, 24, 32, 4, 1, 4, 8, 4, 8
#define TN_V_32 4, 8, 24, 40, 4, 1, 4, 8, 4, 8
#define TN_V_33 4, 8, 32, 16, 4, 1, 4, 8, 4, 8
#define TN_V_34 4, 8, 32, 16, 8, 1, 4, 8, 4, 8
#define TN_V_35 4, 8, 32, 24, 4, 1, 4, 8, 4, 8
#define TN_V_36 4, 8, 40, 16, 4, 1, 4, 8, 4, 8
#define TN_V_37 4, 8, 40, 24, 4, 1, 4, 8, 4, 8
#define TN_V_38 4, 8, 48, 16, 4, 1, 4, 8, 4, 8
#define TN_V_39 4, 8, 56, 16, 4, 1, 4, 8, 4, 8
#define TN_V_40 4, 16, 16, 32, 4, 1, 4, 16, 4, 16
#define TN_V_41 4, 16, 16, 32, 8, 1, 4, 16, 4, 16
#define TN_V_42 4, 16, 16, 32, 12, 1, 4, 16, 4, 16
#define TN_V_43 4, 16, 16, 32, 16, 1, 4, 16, 4, 16
#define TN_V_44 4, 16, 16, 48, 4, 1, 4, 16, 4, 16
#define TN_V_45 4, 16, 16, 48, 8, 1, 4, 16, 4, 16
#define TN_V_46 4, 16, 16, 48, 12, 1, 4, 16, 4, 16
#define TN_V_47 4, 16, 16, 64, 4, 1, 4, 16, 4, 16
#define TN_V_48 4, 16, 16, 64, 8, 1, 4, 16, 4, 16
#define TN_V_49 4, 16, 32, 32, 4, 1, 4, 16, 4, 16
#define TN_V_50 4, 16, 32, 32, 8, 1, 4, 16, 4, 16
#define TN_V_51 4, 16, 32, 32, 12, 1, 4, 16, 4, 16
#define TN_V_52 4, 16, 32, 48, 4, 1, 4, 16, 4, 16
#define TN_V_53 4, 16, 32, 48, 8, 1, 4, 16, 4, 16
#define TN_V_54 4, 16, 48, 32, 4, 1, 4, 16, 4, 16
#define TN_V_55 4, 16, 48, 32, 8, 1, 4, 16, 4, 16
#define TN_V_56 4, 24, 24, 48, 4, 1, 4, 24, 4, 24
#define TN_V_57 4, 24, 24, 48, 8, 1, 4, 24, 4, 24
#define TN_V_58 4, 24, 24, 48, 12, 1, 4, 24, 4, 24
#define TN_V_59 4, 24, 48, 48, 4, 1, 4, 24, 4, 24
#define TN_V_60 4, 24, 48, 48, 8, 1, 4, 24, 4, 24
#define TN_V_61 4, 32, 32, 64, 4, 1, 4, 32, 4, 32
#define TN_V_62 4, 32, 32, 64, 8, 1, 4, 32, 4, 32
#define TN_V_63 4, 32, 32, 64, 12, 1, 4, 32, 4, 32
#define TN_V_64 4, 32, 32, 64, 16, 1, 4, 32, 4, 32
#define TN_V_65 6, 16, 48, 32, 6, 1, 6, 16, 6, 16
#define TN_V_66 6, 16, 48, 32, 12, 1, 6, 16, 6, 16
#define TN_V_67 6, 16, 48, 48, 6, 1, 6, 16, 6, 16
#define TN_V_68 8, 4, 16, 12, 8, 1, 8, 4, 8, 4
#define TN_V_69 8, 4, 16, 16, 8, 1, 8, 4, 8, 4
#define TN_V_70 8, 4, 16, 20, 8, 1, 8, 4, 8, 4
#define TN_V_71 8, 4, 16, 24, 8, 1, 8, 4, 8, 4
#define TN_V_72 8, 4, 16, 28, 8, 1, 8, 4, 8, 4
#define TN_V_73 8, 4, 16, 32, 8, 1, 8, 4, 8, 4
#define TN_V_74 8, 4, 24, 8, 8, 1, 8, 4, 8, 4
#define TN_V_75 8, 4, 24, 12, 8, 1, 8, 4, 8, 4
#define TN_V_76 8, 4, 24, 16, 8, 1, 8, 4, 8, 4
#define TN_V_77 8, 4, 24, 20, 8, 1, 8, 4, 8, 4
#define TN_V_78 8, 4, 24, 24, 8, 1, 8, 4, 8, 4
#define TN_V_79 8, 4, 32, 8, 8, 1, 8, 4, 8, 4
#define TN_V_80 8, 4, 32, 12, 8, 1, 8, 4, 8, 4
#define TN_V_81 8, 4, 32, 16, 8, 1, 8, 4, 8, 4
#define TN_V_82 8, 4, 40, 8, 8, 1, 8, 4, 8, 4
#define TN_V_83 8, 8, 16, 24, 8, 1, 8, 8, 8, 8
#define TN_V_84 8, 8, 16, 24, 16, 1, 8, 8, 8, 8
#define TN_V_85 8, 8, 16, 32, 8, 1, 8, 8, 8, 8
#define TN_V_86 8, 8, 16, 32, 16, 1, 8, 8, 8, 8
#define TN_V_87 8, 8, 16, 40, 8, 1, 8, 8, 8, 8
#define TN_V_88 8, 8, 16, 48, 8, 1, 8, 8, 8, 8
#define TN_V_89 8, 8, 16, 56, 8, 1, 8, 8, 8, 8
#define TN_V_90 8, 8, 16, 64, 8, 1, 8, 8, 8, 8
#define TN_V_91 8, 8, 24, 16, 8, 1, 8, 8, 8, 8
#define TN_V_92 8, 8, 24, 16, 16, 1, 8, 8, 8, 8
#define TN_V_93 8, 8, 24, 24, 8, 1, 8, 8, 8, 8
#define TN_V_94 8, 8, 24, 24, 16, 1, 8, 8, 8, 8
#define TN_V_95 8, 8, 24, 32, 8, 1, 8, 8, 8, 8
#define TN_V_96 8, 8, 24, 40, 8, 1, 8, 8, 8, 8
#define TN_V_97 8, 8, 24, 48, 8, 1, 8, 8, 8, 8
#define TN_V_98 8, 8, 24, 56, 8, 1, 8, 8, 8, 8
#define TN_V_99 8, 8, 24, 64, 8, 1, 8, 8, 8, 8
#define TN_V_100 8, 8, 32, 16, 8, 1, 8, 8, 8, 8
#define TN_V_101 8, 8, 32, 16, 16, 1, 8, 8, 8, 8
#define TN_V_102 8, 8, 32, 24, 8, 1, 8, 8, 8, 8
#define TN_V_103 8, 8, 32, 32, 8, 1, 8, 8, 8, 8
#define TN_V_104 8, 8, 32, 40, 8, 1, 8, 8, 8, 8
#define TN_V_105 8, 8, 32, 48, 8, 1, 8, 8, 8, 8
#define TN_V_106 8, 8, 32, 56, 8, 1, 8, 8, 8, 8
#define TN_V_107 8, 8, 40, 16, 8, 1, 8, 8, 8, 8
#define TN_V_108 8, 8, 40, 24, 8, 1, 8, 8, 8, 8
#define TN_V_109 8, 8, 40, 32, 8, 1, 8, 8, 8, 8
#define TN_V_110 8, 8, 40, 40, 8, 1, 8, 8, 8, 8
#define TN_V_111 8, 8, 40, 48, 8, 1, 8, 8, 8, 8
#define TN_V_112 8, 8, 48, 16, 8, 1, 8, 8, 8, 8
#define TN_V_113 8, 8, 48, 24, 8, 1, 8, 8, 8, 8
#define TN_V_114 8, 8, 48, 32, 8, 1, 8, 8, 8, 8
#define TN_V_115 8, 8, 48, 40, 8, 1, 8, 8, 8, 8
#define TN_V_116 8, 8, 56, 16, 8, 1, 8, 8, 8, 8
#define TN_V_117 8, 8, 56, 24, 8, 1, 8, 8, 8, 8
#define TN_V_118 8, 8, 56, 32, 8, 1, 8, 8, 8, 8
#define TN_V_119 8, 8, 64, 16, 8, 1, 8, 8, 8, 8
#define TN_V_120 8, 8, 64, 24, 8, 1, 8, 8, 8, 8
#define TN_V_121 8, 12, 24, 24, 8, 1, 8, 12, 8, 12
#define TN_V_122 8, 12, 24, 24, 16, 1, 8, 12, 8, 12
#define TN_V_123 8, 12, 24, 36, 8, 1, 8, 12, 8, 12
#define TN_V_124 8, 12, 24, 36, 16, 1, 8, 12, 8, 12
#define TN_V_125 8, 12, 24, 48, 8, 1, 8, 12, 8, 12
#define TN_V_126 8, 12, 24, 60, 8, 1, 8, 12, 8, 12
#define TN_V_127 8, 12, 48, 24, 8, 1, 8, 12, 8, 12
#define TN_V_128 8, 12, 48, 36, 8, 1, 8, 12, 8, 12
#define TN_V_129 8, 12, 48, 48, 8, 1, 8, 12, 8, 12
#define TN_V_130 8, 12, 48, 60, 8, 1, 8, 12, 8, 12
#define TN_V_131 8, 16, 16, 48, 8, 1, 8, 16, 8, 16
#define TN_V_132 8, 16, 16, 48, 16, 1, 8, 16, 8, 16
#define TN_V_133 8, 16, 16, 48, 24, 1, 8, 16, 8, 16
#define TN_V_134 8, 16, 16, 64, 8, 1, 8, 16, 8, 16
#define TN_V_135 8, 16, 16, 64, 16, 1, 8, 16, 8, 16
#define TN_V_136 8, 16, 32, 32, 8, 1, 8, 16, 8, 16
#define TN_V_137 8, 16, 32, 32, 16, 1, 8, 16, 8, 16
#define TN_V_138 8, 16, 32, 32, 24, 1, 8, 16, 8, 16
#define TN_V_139 8, 16, 32, 48, 8, 1, 8, 16, 8, 16
#define TN_V_140 8, 16, 32, 48, 16, 1, 8, 16, 8, 16
#define TN_V_141 8, 16, 32, 64, 8, 1, 8, 16, 8, 16
#define TN_V_142 8, 16, 32, 64, 16, 1, 8, 16, 8, 16
#define TN_V_143 8, 16, 48, 32, 8, 1, 8, 16, 8, 16
#define TN_V_144 8, 16, 48, 32, 16, 1, 8, 16, 8, 16
#define TN_V_145 8, 16, 48, 48, 8, 1, 8, 16, 8, 16
#define TN_V_146 8, 16, 48, 48, 16, 1, 8, 16, 8, 16
#define TN_V_147 8, 16, 48, 64, 8, 1, 8, 16, 8, 16
#define TN_V_148 8, 16, 64, 32, 8, 1, 8, 16, 8, 16
#define TN_V_149 8, 16, 64, 32, 16, 1, 8, 16, 8, 16
#define TN_V_150 8, 16, 64, 48, 8, 1, 8, 16, 8, 16
#define TN_V_151 8, 20, 40, 40, 8, 1, 8, 20, 8, 20
#define TN_V_152 8, 20, 40, 40, 16, 1, 8, 20, 8, 20
#define TN_V_153 8, 20, 40, 60, 8, 1, 8, 20, 8, 20
#define TN_V_154 8, 24, 24, 48, 8, 1, 8, 24, 8, 24
#define TN_V_155 8, 24, 24, 48, 16, 1, 8, 24, 8, 24
#define TN_V_156 8, 24, 24, 48, 24, 1, 8, 24, 8, 24
#define TN_V_157 8, 24, 48, 48, 8, 1, 8, 24, 8, 24
#define TN_V_158 8, 24, 48, 48, 16, 1, 8, 24, 8, 24
#define TN_V_159 8, 28, 56, 56, 8, 1, 8, 28, 8, 28
#define TN_V_160 8, 28, 56, 56, 16, 1, 8, 28, 8, 28
#define TN_V_161 8, 32, 32, 64, 8, 1, 8, 32, 8, 32
#define TN_V_162 8, 32, 32, 64, 16, 1, 8, 32, 8, 32
#define TN_V_163 8, 32, 32, 64, 24, 1, 8, 32, 8, 32
#define TN_V_164 8, 32, 32, 64, 32, 1, 8, 32, 8, 32
#define TN_V_165 8, 32, 64, 64, 8, 1, 8, 32, 8, 32
#define TN_V_166 8, 32, 64, 64, 16, 1, 8, 32, 8, 32
#define TN_V_167 8, 32, 64, 64, 24, 1, 8, 32, 8, 32
#define TN_V_168 12, 8, 24, 24, 12, 1, 12, 8, 12, 8
#define TN_V_169 12, 8, 24, 32, 12, 1, 12, 8, 12, 8
#define TN_V_170 12, 8, 24, 40, 12, 1, 12, 8, 12, 8
#define TN_V_171 12, 8, 24, 48, 12, 1, 12, 8, 12, 8
#define TN_V_172 12, 8, 24, 56, 12, 1, 12, 8, 12, 8
#define TN_V_173 12, 8, 48, 16, 12, 1, 12, 8, 12, 8
#define TN_V_174 12, 8, 48, 24, 12, 1, 12, 8, 12, 8
#define TN_V_175 12, 8, 48, 32, 12, 1, 12, 8, 12, 8
#define TN_V_176 12, 16, 48, 32, 12, 1, 12, 16, 12, 16
#define TN_V_177 12, 16, 48, 32, 24, 1, 12, 16, 12, 16
#define TN_V_178 12, 16, 48, 48, 12, 1, 12, 16, 12, 16
#define TN_V_179 12, 16, 48, 64, 12, 1, 12, 16, 12, 16
#define TN_V_180 12, 24, 48, 48, 12, 1, 12, 24, 12, 24
#define TN_V_181 12, 24, 48, 48, 24, 1, 12, 24, 12, 24
#define TN_V_182 16, 4, 32, 12, 16, 1, 16, 4, 16, 4
#define TN_V_183 16, 4, 32, 16, 16, 1, 16, 4, 16, 4
#define TN_V_184 16, 6, 48, 12, 16, 1, 16, 6, 16, 6
#define TN_V_185 16, 8, 32, 24, 16, 1, 16, 8, 16, 8
#define TN_V_186 16, 8, 32, 32, 16, 1, 16, 8, 16, 8
#define TN_V_187 16, 8, 32, 40, 16, 1, 16, 8, 16, 8
#define TN_V_188 16, 8, 32, 48, 16, 1, 16, 8, 16, 8
#define TN_V_189 16, 8, 32, 56, 16, 1, 16, 8, 16, 8
#define TN_V_190 16, 8, 32, 64, 16, 1, 16, 8, 16, 8
#define TN_V_191 16, 8, 48, 16, 16, 1, 16, 8, 16, 8
#define TN_V_192 16, 8, 48, 24, 16, 1, 16, 8, 16, 8
#define TN_V_193 16, 8, 48, 32, 16, 1, 16, 8, 16, 8
#define TN_V_194 16, 8, 48, 40, 16, 1, 16, 8, 16, 8
#define TN_V_195 16, 8, 48, 48, 16, 1, 16, 8, 16, 8
#define TN_V_196 16, 8, 64, 16, 16, 1, 16, 8, 16, 8
#define TN_V_197 16, 8, 64, 24, 16, 1, 16, 8, 16, 8
#define TN_V_198 16, 8, 64, 32, 16, 1, 16, 8, 16, 8
#define TN_V_199 16, 12, 48, 24, 16, 1, 16, 12, 16, 12
#define TN_V_200 16, 12, 48, 36, 16, 1, 16, 12, 16, 12
#define TN_V_201 16, 12, 48, 48, 16, 1, 16, 12, 16, 12
#define TN_V_202 16, 12, 48, 60, 16, 1, 16, 12, 16, 12
#define TN_V_203 16, 16, 32, 48, 16, 1, 16, 16, 16, 16
#define TN_V_204 16, 16, 32, 48, 32, 1, 16, 16, 16, 16
#define TN_V_205 16, 16, 32, 64, 16, 1, 16, 16, 16, 16
#define TN_V_206 16, 16, 32, 64, 32, 1, 16, 16, 16, 16
#define TN_V_207 16, 16, 48, 32, 16, 1, 16, 16, 16, 16
#define TN_V_208 16, 16, 48, 32, 32, 1, 16, 16, 16, 16
#define TN_V_209 16, 16, 48, 48, 16, 1, 16, 16, 16, 16
#define TN_V_210 16, 16, 48, 48, 32, 1, 16, 16, 16, 16
#define TN_V_211 16, 16, 48, 64, 16, 1, 16, 16, 16, 16
#define TN_V_212 16, 16, 64, 32, 16, 1, 16, 16, 16, 16
#define TN_V_213 16, 16, 64, 32, 32, 1, 16, 16, 16, 16
#define TN_V_214 16, 16, 64, 48, 16, 1, 16, 16, 16, 16
#define TN_V_215 16, 16, 64, 64, 16, 1, 16, 16, 16, 16
#define TN_V_216 16, 24, 48, 48, 16, 1, 16, 24, 16, 24
#define TN_V_217 16, 24, 48, 48, 32, 1, 16, 24, 16, 24
#define TN_V_218 16, 32, 64, 64, 16, 1, 16, 32, 16, 32
#define TN_V_219 16, 32, 64, 64, 32, 1, 16, 32, 16, 32
#define TN_V_220 16, 32, 64, 64, 48, 1, 16, 32, 16, 32
#define TN_V_221 20, 8, 40, 24, 20, 1, 20, 8, 20, 8
#define TN_V_222 20, 8, 40, 32, 20, 1, 20, 8, 20, 8
#define TN_V_223 24, 8, 48, 24, 24, 1, 24, 8, 24, 8
#define TN_V_224 24, 8, 48, 32, 24, 1, 24, 8, 24, 8
#define TN_V_225 24, 12, 48, 36, 24, 1, 24, 12, 24, 12
#define TN_V_226 24, 12, 48, 48, 24, 1, 24, 12, 24, 12
#define TN_V_227 24, 12, 48, 60, 24, 1, 24, 12, 24, 12
#define TN_V_228 24, 16, 48, 48, 24, 1, 24, 16, 24, 16
#define TN_V_229 24, 16, 48, 64, 24, 1, 24, 16, 24, 16
#define TN_V_230 32, 8, 64, 24, 32, 1, 32, 8, 32, 8
#define TN_V_231 32, 8, 64, 32, 32, 1, 32, 8, 32, 8
#define TN_V_232 32, 16, 64, 48, 32, 1, 32, 16, 32, 16
#define TN_V_233 32, 16, 64, 64, 32, 1, 32, 16, 32, 16
#define TN_V_234 2, 16, 16, 32, 4, 2, 2, 16, 2, 16
#define TN_V_235 2, 16, 16, 32, 8, 2, 2, 16, 2, 16
#define TN_V_236 2, 16, 16, 48, 4, 2, 2, 16, 2, 16
#define TN_V_237 4, 8, 8, 24, 8, 2, 4, 8, 4, 8
#define TN_V_238 4, 8, 8, 32, 8, 2, 4, 8, 4, 8
#define TN_V_239 4, 8, 8, 40, 8, 2, 4, 8, 4, 8
#define TN_V_240 4, 8, 16, 16, 8, 2, 4, 8, 4, 8
#define TN_V_241 4, 8, 16, 24, 8, 2, 4, 8, 4, 8
#define TN_V_242 4, 8, 16, 32, 8, 2, 4, 8, 4, 8
#define TN_V_243 4, 8, 24, 16, 8, 2, 4, 8, 4, 8
#define TN_V_244 4, 8, 24, 24, 8, 2, 4, 8, 4, 8
#define TN_V_245 4, 8, 32, 16, 8, 2, 4, 8, 4, 8
#define TN_V_246 4, 16, 16, 32, 8, 2, 4, 16, 4, 16
#define TN_V_247 4, 16, 16, 32, 16, 2, 4, 16, 4, 16
#define TN_V_248 4, 16, 16, 48, 8, 2, 4, 16, 4, 16
#define TN_V_249 4, 16, 16, 64, 8, 2, 4, 16, 4, 16
#define TN_V_250 4, 16, 32, 32, 8, 2, 4, 16, 4, 16
#define TN_V_251 4, 16, 32, 48, 8, 2, 4, 16, 4, 16
#define TN_V_252 4, 16, 48, 32, 8, 2, 4, 16, 4, 16
#define TN_V_253 4, 24, 24, 48, 8, 2, 4, 24, 4, 24
#define TN_V_254 4, 24, 48, 48, 8, 2, 4, 24, 4, 24
#define TN_V_255 4, 32, 32, 64, 8, 2, 4, 32, 4, 32
#define TN_V_256 4, 32, 32, 64, 16, 2, 4, 32, 4, 32
#define TN_V_257 6, 16, 48, 32, 12, 2, 6, 16, 6, 16
#define TN_V_258 8, 8, 16, 24, 16, 2, 8, 8, 8, 8
#define TN_V_259 8, 8, 16, 32, 16, 2, 8, 8, 8, 8
#define TN_V_260 8, 8, 32, 16, 16, 2, 8, 8, 8, 8
#define TN_V_261 8, 16, 16, 48, 16, 2, 8, 16, 8, 16
#define TN_V_262 8, 16, 16, 64, 16, 2, 8, 16, 8, 16
#define TN_V_263 8, 16, 32, 32, 16, 2, 8, 16, 8, 16
#define TN_V_264 8, 16, 32, 48, 16, 2, 8, 16, 8, 16
#define TN_V_265 8, 16, 32, 64, 16, 2, 8, 16, 8, 16
#define TN_V_266 8, 16, 48, 32, 16, 2, 8, 16, 8, 16
#define TN_V_267 8, 16, 48, 48, 16, 2, 8, 16, 8, 16
#define TN_V_268 8, 16, 64, 32, 16, 2, 8, 16, 8, 16
#define TN_V_269 8, 24, 48, 48, 16, 2, 8, 24, 8, 24
#define TN_V_270 8, 32, 32, 64, 16, 2, 8, 32, 8, 32
#define TN_V_271 8, 32, 32, 64, 32, 2, 8, 32, 8, 32
#define TN_V_272 8, 32, 64, 64, 16, 2, 8, 32, 8, 32
#define TN_V_273 12, 16, 48, 32, 24, 2, 12, 16, 12, 16
#define TN_V_274 12, 24, 48, 48, 24, 2, 12, 24, 12, 24
#define TN_V_275 16, 16, 32, 48, 32, 2, 16, 16, 16, 16
#define TN_V_276 16, 16, 32, 64, 32, 2, 16, 16, 16, 16
#define TN_V_277 16, 16, 64, 32, 32, 2, 16, 16, 16, 16
#define TN_V_278 16, 32, 64, 64, 32, 2, 16, 32, 16, 32

#endif

#endif