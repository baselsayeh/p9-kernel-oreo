/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and 
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may 
 * *    be used to endorse or promote products derived from this software 
 * *    without specific prior written permission.
 * 
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : CbtCpm.c
  �� �� ��   : ����
  ��    ��   :
  ��������   :
  ����޸�   :
  ��������   : CBT�������˿ں��߼�ͨ��ӳ�����ģ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��5��31��
    ��    ��   : L00256032
    �޸�����   : V8R1 OM_Optimize��Ŀ����

***************************************************************************** */

/*****************************************************************************
  1 ͷ�ļ�����
**************************************************************************** */
#include "CbtCpm.h"
#include "cpm.h"
#include "SCMSoftDecode.h"



/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_CBT_CPM_C
/*lint +e767 */

/* ****************************************************************************
  2 ȫ�ֱ�������
**************************************************************************** */
CBTCPM_RCV_FUNC                g_pCbtRcvFunc = VOS_NULL_PTR;
CBTCPM_SEND_FUNC               g_pCbtSndFunc = VOS_NULL_PTR;

/*****************************************************************************
  3 �ⲿ��������
*****************************************************************************/

extern VOS_UINT32 CBTSCM_SoftDecodeDataRcv(VOS_UINT8 *pucBuffer, VOS_UINT32 ulLen);

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : CBTCPM_PortRcvReg
 ��������  : ��CBTͨ��ע����պ���
 �������  : pRecvFunc��   ���ݽ��պ�����ַ
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
VOS_VOID CBTCPM_PortRcvReg(CBTCPM_RCV_FUNC pRcvFunc)
{
    g_pCbtRcvFunc = pRcvFunc;

    return;
}

/*****************************************************************************
 �� �� ��  : CBTCPM_PortSndReg
 ��������  : ��CBTͨ��ע�ᷢ�ͺ���
 �������  : pSndFunc��   ���ݷ��ͺ�����ַ
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
VOS_VOID CBTCPM_PortSndReg(CBTCPM_SEND_FUNC pSndFunc)
{
    g_pCbtSndFunc = pSndFunc;

    return;
}

/*****************************************************************************
 �� �� ��  : CBTCPM_GetRcvFunc
 ��������  : ��ȡCBTͨ�����պ�����ַ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ���ݽ��պ�����ַ
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
CBTCPM_RCV_FUNC CBTCPM_GetRcvFunc(VOS_VOID)
{
    return g_pCbtRcvFunc;
}

/*****************************************************************************
 �� �� ��  : CBTCPM_GetSndFunc
 ��������  : ��ȡCBTͨ�����ͺ�����ַ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ���ݷ��ͺ�����ַ
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
CBTCPM_SEND_FUNC CBTCPM_GetSndFunc(VOS_VOID)
{
    return g_pCbtSndFunc;
}

/*****************************************************************************
 �� �� ��  : CBTCPM_NotifyChangePort
 ��������  : AT���CBT�����ݽ��պͷ��ͺ���
 �������  : enPhyPort:AT�������˿ں�
 �������  : ��
 �� �� ֵ  : VOS_OK/VOS_ERR
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
VOS_UINT32 CBTCPM_NotifyChangePort(AT_PHY_PORT_ENUM_UINT32 enPhyPort)
{
    /* NAS��UART����У׼ */
    if (CPM_IND_PORT > enPhyPort)
    {
        /* ��AT��÷������ݵĺ���ָ�� */
        CBTCPM_PortSndReg(AT_QuerySndFunc(enPhyPort));

        /* ��AT�Ķ˿���У׼������USB��VCOM�������� */
        CBTCPM_PortRcvReg(VOS_NULL_PTR);

        /* ��У׼ͨ���Ľ��պ�����ATģ�� */
        AT_RcvFuncReg(enPhyPort, CBTSCM_SoftDecodeDataRcv);

        return VOS_OK;
    }

    return VOS_ERR;
}




