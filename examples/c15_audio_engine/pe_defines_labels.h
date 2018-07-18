/******************************************************************************/
/** @file       pe_defines_labels.h
    @date       2018-04-20
    @version    1.0
    @author     Matthias Seeber
    @brief      c15 parameter labels (for better code readability)
                NOTE:
                The provided IDs represent the rendering items of the parameter
                engine, NOT the elements of the shared m_paramsignaldata[][] array!
                ..
    @todo
*******************************************************************************/

#pragma once

/* Envelope Parameter Labels - access parameter with (envIndex + ID) */

#define E_ATT       0
#define E_DEC1      1
#define E_BP        2
#define E_DEC2      3
#define E_SUS       4
#define E_REL       5
#define E_LV        7
#define E_AV        8
#define E_RV        9
#define E_LKT       10
#define E_TKT       11
#define E_AC        12
#define E_RH        13
#define E_SPL       14

/* 'normal' Parameter Labels - access parameter with m_head[ID] - maybe, the rendering index would be much more efficient? */

#define P_EA_GAIN   6
#define P_EB_GAIN   21

#define P_OA_P      45
#define P_OA_PKT    46
#define P_OA_PEC    47
#define P_OA_F      48
#define P_OA_FEC    49
#define P_OA_PMS    50
#define P_OA_PMSEA  51
#define P_OA_PMSSH  52
#define P_OA_PMB    53
#define P_OA_PMBEB  54
#define P_OA_PMBSH  55
#define P_OA_PMF    56
#define P_OA_PMFEC  57
#define P_OA_CHI    58

#define P_SA_D      59
#define P_SA_DEA    60
#define P_SA_F      61
#define P_SA_A      62
#define P_SA_M      63
#define P_SA_FBM    64
#define P_SA_FBEC   65
#define P_SA_RM     66

#define P_OB_P      67
#define P_OB_PKT    68
#define P_OB_PEC    69
#define P_OB_F      70
#define P_OB_FEC    71
#define P_OB_PMS    72
#define P_OB_PMSEB  73
#define P_OB_PMSSH  74
#define P_OB_PMA    75
#define P_OB_PMAEA  76
#define P_OB_PMASH  77
#define P_OB_PMF    78
#define P_OB_PMFEC  79
#define P_OB_CHI    80

#define P_SB_D      81
#define P_SB_DEB    82
#define P_SB_F      83
#define P_SB_A      84
#define P_SB_M      85
#define P_SB_FBM    86
#define P_SB_FBEC   87
#define P_SB_RM     88

#define P_CMB_AB    89
#define P_CMB_P     90
#define P_CMB_PKT   91
#define P_CMB_PEC   92
#define P_CMB_D     93
#define P_CMB_DKT   94
#define P_CMB_DG    95
#define P_CMB_APT   96
#define P_CMB_APKT  97
#define P_CMB_APEC  98
#define P_CMB_APR   99
#define P_CMB_LP    100
#define P_CMB_LPKT  101
#define P_CMB_LPEC  102
#define P_CMB_PM    103
#define P_CMB_PMAB  104

#define P_SVF_AB    105
#define P_SVF_CMB   106
#define P_SVF_CUT   107
#define P_SVF_CKT   108
#define P_SVF_CEC   109
#define P_SVF_SPR   110
#define P_SVF_FM    111
#define P_SVF_RES   112
#define P_SVF_RKT   113
#define P_SVF_REC   114
#define P_SVF_LBH   115
#define P_SVF_PAR   116
#define P_SVF_FMAB  117

#define P_FBM_CMB   118
#define P_FBM_SVF   119
#define P_FBM_FX    120
#define P_FBM_REV   121
#define P_FBM_DRV   122
#define P_FBM_FLD   123
#define P_FBM_ASM   124
#define P_FBM_LKT   125
#define P_FBM_LVL   126

#define P_OM_AL     127
#define P_OM_AP     128
#define P_OM_BL     129
#define P_OM_BP     130
#define P_OM_CL     131
#define P_OM_CP     132
#define P_OM_SL     133
#define P_OM_SP     134
#define P_OM_DRV    135
#define P_OM_FLD    136
#define P_OM_ASM    137
#define P_OM_LVL    138

#define P_CAB_DRV   139
#define P_CAB_FLD   140
#define P_CAB_ASM   141
#define P_CAB_TILT  142
#define P_CAB_LPF   143
#define P_CAB_HPF   144
#define P_CAB_LVL   145
#define P_CAB_MIX   146

#define P_GAP_CNT   147
#define P_GAP_STE   148
#define P_GAP_GAP   149
#define P_GAP_RES   150
#define P_GAP_BAL   151
#define P_GAP_MIX   152

#define P_FLA_TMOD  153
#define P_FLA_PHS   154
#define P_FLA_RTE   155
#define P_FLA_TIME  156
#define P_FLA_STE   157
#define P_FLA_FB    158
#define P_FLA_CFB   159
#define P_FLA_LPF   160
#define P_FLA_MIX   161
#define P_FLA_ENV   162
#define P_FLA_APM   163
#define P_FLA_APT   164

#define P_DLY_TIME  165
#define P_DLY_STE   166
#define P_DLY_FB    167
#define P_DLY_CFB   168
#define P_DLY_LPF   169
#define P_DLY_MIX   170

#define P_REV_SIZE  171
#define P_REV_PRE   172
#define P_REV_COL   173
#define P_REV_CHO   174
#define P_REV_MIX   175

#define P_MA_V      176
#define P_MA_T      177

#define P_KEY_PA    178
#define P_KEY_PB    179
#define P_KEY_NP    180
#define P_KEY_VP    181
#define P_KEY_EC    182
#define P_KEY_VS    183
