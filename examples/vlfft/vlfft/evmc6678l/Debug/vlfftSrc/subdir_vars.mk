################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
SA_SRCS += \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/DSPF_sp_mixedRadix_fftSPXSP.sa \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/multiTwiddle_1_sa.sa \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/transpose_2Cols_rowsX8_cplxMatrix_sa.sa \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/transpose_2Rows_8XCols_cplxMatrix_sa.sa 

ASM_SRCS += \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/DSPF_sp_radix4_fftSPxSP.asm \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/tsc_h.asm 

C_SRCS += \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/dft.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/dmaParamInit.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/genTwiddle.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/messgQUtil.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/multTwiddle.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfftParamsInit.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfftUtil.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfft_1stIter.c \
C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfft_2ndIter.c 

OBJS += \
./vlfftSrc/DSPF_sp_mixedRadix_fftSPXSP.obj \
./vlfftSrc/DSPF_sp_radix4_fftSPxSP.obj \
./vlfftSrc/dft.obj \
./vlfftSrc/dmaParamInit.obj \
./vlfftSrc/genTwiddle.obj \
./vlfftSrc/messgQUtil.obj \
./vlfftSrc/multTwiddle.obj \
./vlfftSrc/multiTwiddle_1_sa.obj \
./vlfftSrc/transpose_2Cols_rowsX8_cplxMatrix_sa.obj \
./vlfftSrc/transpose_2Rows_8XCols_cplxMatrix_sa.obj \
./vlfftSrc/tsc_h.obj \
./vlfftSrc/vlfftParamsInit.obj \
./vlfftSrc/vlfftUtil.obj \
./vlfftSrc/vlfft_1stIter.obj \
./vlfftSrc/vlfft_2ndIter.obj 

ASM_DEPS += \
./vlfftSrc/DSPF_sp_radix4_fftSPxSP.pp \
./vlfftSrc/tsc_h.pp 

C_DEPS += \
./vlfftSrc/dft.pp \
./vlfftSrc/dmaParamInit.pp \
./vlfftSrc/genTwiddle.pp \
./vlfftSrc/messgQUtil.pp \
./vlfftSrc/multTwiddle.pp \
./vlfftSrc/vlfftParamsInit.pp \
./vlfftSrc/vlfftUtil.pp \
./vlfftSrc/vlfft_1stIter.pp \
./vlfftSrc/vlfft_2ndIter.pp 

SA_DEPS += \
./vlfftSrc/DSPF_sp_mixedRadix_fftSPXSP.pp \
./vlfftSrc/multiTwiddle_1_sa.pp \
./vlfftSrc/transpose_2Cols_rowsX8_cplxMatrix_sa.pp \
./vlfftSrc/transpose_2Rows_8XCols_cplxMatrix_sa.pp 

C_DEPS__QUOTED += \
"vlfftSrc\dft.pp" \
"vlfftSrc\dmaParamInit.pp" \
"vlfftSrc\genTwiddle.pp" \
"vlfftSrc\messgQUtil.pp" \
"vlfftSrc\multTwiddle.pp" \
"vlfftSrc\vlfftParamsInit.pp" \
"vlfftSrc\vlfftUtil.pp" \
"vlfftSrc\vlfft_1stIter.pp" \
"vlfftSrc\vlfft_2ndIter.pp" 

OBJS__QUOTED += \
"vlfftSrc\DSPF_sp_mixedRadix_fftSPXSP.obj" \
"vlfftSrc\DSPF_sp_radix4_fftSPxSP.obj" \
"vlfftSrc\dft.obj" \
"vlfftSrc\dmaParamInit.obj" \
"vlfftSrc\genTwiddle.obj" \
"vlfftSrc\messgQUtil.obj" \
"vlfftSrc\multTwiddle.obj" \
"vlfftSrc\multiTwiddle_1_sa.obj" \
"vlfftSrc\transpose_2Cols_rowsX8_cplxMatrix_sa.obj" \
"vlfftSrc\transpose_2Rows_8XCols_cplxMatrix_sa.obj" \
"vlfftSrc\tsc_h.obj" \
"vlfftSrc\vlfftParamsInit.obj" \
"vlfftSrc\vlfftUtil.obj" \
"vlfftSrc\vlfft_1stIter.obj" \
"vlfftSrc\vlfft_2ndIter.obj" 

SA_DEPS__QUOTED += \
"vlfftSrc\DSPF_sp_mixedRadix_fftSPXSP.pp" \
"vlfftSrc\multiTwiddle_1_sa.pp" \
"vlfftSrc\transpose_2Cols_rowsX8_cplxMatrix_sa.pp" \
"vlfftSrc\transpose_2Rows_8XCols_cplxMatrix_sa.pp" 

ASM_DEPS__QUOTED += \
"vlfftSrc\DSPF_sp_radix4_fftSPxSP.pp" \
"vlfftSrc\tsc_h.pp" 

SA_SRCS__QUOTED += \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/DSPF_sp_mixedRadix_fftSPXSP.sa" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/multiTwiddle_1_sa.sa" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/transpose_2Cols_rowsX8_cplxMatrix_sa.sa" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/transpose_2Rows_8XCols_cplxMatrix_sa.sa" 

ASM_SRCS__QUOTED += \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/DSPF_sp_radix4_fftSPxSP.asm" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/tsc_h.asm" 

C_SRCS__QUOTED += \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/dft.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/dmaParamInit.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/genTwiddle.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/messgQUtil.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/multTwiddle.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfftParamsInit.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfftUtil.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfft_1stIter.c" \
"C:/dev/keystone-workshop/trunk/examples/vlfft/vlfft/vlfftSrc/vlfft_2ndIter.c" 


