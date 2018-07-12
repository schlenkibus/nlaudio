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

#define P_OM_AL     118
#define P_OM_AP     119
#define P_OM_BL     120
#define P_OM_BP     121
#define P_OM_CL     122
#define P_OM_CP     123
#define P_OM_SL     124
#define P_OM_SR     125
#define P_OM_DRV    126
#define P_OM_FLD    127
#define P_OM_ASM    128
#define P_OM_LVL    129

#define P_MA_V      130
#define P_MA_T      131

#define P_KEY_PA    132
#define P_KEY_PB    133
#define P_KEY_NP    134
#define P_KEY_VP    135
#define P_KEY_EC    136
#define P_KEY_VS    137
