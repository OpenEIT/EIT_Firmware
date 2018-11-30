#ifndef __AFE_LIB_H__
#define __AFE_LIB_H__

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* AFE high level example function prototypes */
extern ADI_AFE_RESULT_TYPE      adi_AFE_PowerUp                         (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_AuxChanCal                      (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_AuxChanMeasFixed                (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         int32_t *const                 measResult);
extern ADI_AFE_RESULT_TYPE      adi_AFE_AuxChanMeasCustom               (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         uint32_t                       muxChannel, 
                                                                         uint16_t *const                measResult);
extern ADI_AFE_RESULT_TYPE      adi_AFE_TempSensChanCal                 (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_TempSensMeas                    (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         int32_t *const                 measResult);
extern ADI_AFE_RESULT_TYPE      adi_AFE_ExciteChanPowerUp               (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_TiaChanCal                      (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_ExciteChanCalAtten              (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_ExciteChanCalNoAtten            (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_AcMeas10khz                     (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         int16_t *const                 dftResults);
extern ADI_AFE_RESULT_TYPE	    adi_AFE_AcMeas                          (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         uint32_t                       fcw, 
                                                                         uint16_t                       amplitude, 
                                                                         bool_t                         atten, 
                                                                         uint16_t                       swState, 
                                                                         int16_t                        *dftResults);
extern ADI_AFE_RESULT_TYPE      adi_AFE_DcMeas                          (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         uint16_t *const                measBuffer,
                                                                         uint16_t                       measBufferLength);
extern ADI_AFE_RESULT_TYPE      adi_AFE_PowerDown                       (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_TiaChanDacCode                  (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         uint32_t*                      pTiaChanDacCode);
extern ADI_AFE_RESULT_TYPE      adi_AFE_ExciteChanNoAttenDeltaDacCode   (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         uint32_t*                      pExciteChanNoAttenDeltaDacCode);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
