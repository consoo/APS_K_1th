#include "StdAfx.h"
#include "PcbProcess2.h"

CWinThread* pThread_TaskCCD = NULL;
bool bThreadCCDRun = false;
typedef struct THREADV
{
	int t_Unit;
}_threadV;
//
UINT Thread_TaskCCD(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;
	CString logStr = "";


	bThreadCCDRun = true;
	if (!pFrame->MIUCheck_process())// || gMIUDevice.CurrentState != 4)
	{
		logStr.Format("CCD ¸ðµâ ¿µ»ó ÃÊ±âÈ­ ½ÇÆÐ.\n Á¦Ç° ¾ÈÂø »óÅÂ ¹× Á¦Ç° ºÒ·® È®ÀÎ ÇÏ¼¼¿ä.");
		errMsg2(Task.AutoFlag, logStr);
		bThreadCCDRun = false;
		return false;
	}
	Sleep(300);
	bThreadCCDRun = false;


	return true;
}
CPcbProcess2::CPcbProcess2(void)
{
	dwTickStartRun = false;

}


CPcbProcess2::~CPcbProcess2(void)
{
}

void CPcbProcess2::putListLog(CString strLog)
{
	theApp.MainDlg->putListLog(strLog);
}
int CPcbProcess2::Ready_process(int iStep)
{
	CString sLog = "";
	int iRtnFunction = iStep;

	switch (iStep)
	{
	case 10000:
		Task.PCBTaskTime = myTimer(true);
		vision.clearOverlay(CCD);

		if (sysData.m_iIrChartUse == 1)
		{
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, 0);
			Sleep(100);
			LightControlSecond.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP2_CHART]);
		}
		else
		{
			LightControlSecond.ctrlLedVolume(LIGHT_TOP_CHART1, 0);
			Sleep(100);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP1_CHART]);
		}
		iRtnFunction = 10150;
		break;


	case  10150:
		if (Dio.LensMotorGripCheck(true, false))
		{
			if (Task.m_bOKLensPass == 1)   //Lens ³Ñ±è ¿Ï·á»óÅÂ
			{
				iRtnFunction = 10160;
				sLog.Format("		Ready Step [%d]", iRtnFunction);
				putListLog(sLog);
			}
			else
			{
				sLog.Format("[¿îÀü ÁØºñ] Lens Grip ÀüÁø »óÅÂ ÀÔ´Ï´Ù[%d]\n·»Áî³Ñ±è»óÅÂ È®ÀÎ¹Ù¶ø´Ï´Ù", iStep);
				errMsg2(Task.AutoFlag, sLog);
				iRtnFunction = -10150;
				break;
			}
		}
		else
		{
			if (!Dio.LensMotorGripCheck(false, false))
			{
				sLog.Format("[¿îÀü ÁØºñ]Lens Grip ÀüÁø È®ÀÎ ½ÇÆÐ [%d]", iStep);
				errMsg2(Task.AutoFlag, sLog);
				iRtnFunction = -10150;
			}
			else
			{
				if (Task.m_bOKLensPass == 1)   //Lens ³Ñ±è ¿Ï·á»óÅÂ
				{
					sLog.Format("[¿îÀü ÁØºñ]Lens Grip ÈÄÁø »óÅÂ [%d]]\n·»Áî³Ñ±è»óÅÂ È®ÀÎ¹Ù¶ø´Ï´Ù", iStep);
					errMsg2(Task.AutoFlag, sLog);
					iRtnFunction = -10150;
				}
				else
				{
					iRtnFunction = 10160;
					sLog.Format("		Ready Step [%d]", iRtnFunction);
					putListLog(sLog);
				}

			}
		}
		break;

	case 10160:

		iRtnFunction = 10170;
		break;

	case 10170:
		iRtnFunction = 10200;
		break;

	case 10200:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))		//PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿
		{
			iRtnFunction = 10300;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING305);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] PCB ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10200;
		}

		break;

	case 10300:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))		//Lens ZÃà ´ë±â À§Ä¡
		{
			iRtnFunction = 10400;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING303);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] Lens ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10300;
		}
		break;
	case 10400:
		if (motor.Lens_LaserAlign_Motor_MoveZ(Wait_Pos))		//Lens ZÃà ´ë±â À§Ä¡
		{
			iRtnFunction = 10600;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1504);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] Lens ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10400;
		}
		break;
	case 10600:
		if (motor.Lens_Motor_MoveX(Wait_Pos))		//Lens X Ãà ´ë±â À§Ä¡
		{
			iRtnFunction = 10601;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1504);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] Lens ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10600;
		}
		break;
	case 10601:
		if (motor.INSP_Motor_MoveX(Wait_Pos))
		{
			iRtnFunction = 10602;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1499);	//LENSºÎ Tx, TyÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			sLog.Format(sLangChange, MotorPosName[Wait_Pos], iRtnFunction);
			putListLog(sLog);
			iRtnFunction = -10601;
		}
		break;
	case 10602:
		if (motor.Pcb_Motor_Move(Wait_Pos))			//PCBºÎ ¸ðÅÍ ´ë±âÀ§Ä¡ ÀÌµ¿
		{
			iRtnFunction = 10700;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING306);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] PCB ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10602;
		}
		break;
	case 10700:
		iRtnFunction = 10701;
		break;
	case 10701:
		if (motor.Lens_Motor_MoveXY(2, Wait_Pos))			//LensºÎ ¸ðÅÍ ´ë±â À§Ä¡ ÀÌµ¿(Lens X/Y/ZÃà)
		{
			iRtnFunction = 10750;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING304);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] Lens ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10701;
		}
		break;
	case 10750:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10800;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1017);	//PCBºÎ Tx, TyÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			sLog.Format(sLangChange);
			putListLog(sLog);
			iRtnFunction = -10750;
		}
		break;
	case 10800:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10900;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING745);	//LENSºÎ Tx, TyÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			sLog.Format(sLangChange);
			putListLog(sLog);
			iRtnFunction = -10800;
		}
		break;
	case  10900:
		//Dio.LensVacuumOn(false);
		iRtnFunction = 10901;
		break;
	case  10901:
		if (motor.Lens_LaserAlign_Motor_Move(Wait_Pos))  //Lens_LaserAlign_Tilt_Move
		{
			iRtnFunction = 10950;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1505);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] Lens ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10900;
		}
		break;
	case 10950:
		if (motor.Lens_LaserAlign_Tilt_Move(Wait_Pos))  //Lens_LaserAlign_Tilt_Move
		{
			iRtnFunction = 10970;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1506);
			sLog.Format(sLangChange, iStep);	//"[¿îÀü ÁØºñ] Lens ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]"
			putListLog(sLog);
			iRtnFunction = -10950;
			break;
		}
		break;
	case 10970:
		MIU.Close();
		iRtnFunction = 15000;	//While¹® Á¾·á
		break;
	case 15000:
		UVCommand.UV_Shutter_PowerSet(model.UV_Power);// 95);//
		sLog.Format("	#1 UV POWER SET: %d", model.UV_Power);
		putListLog(sLog);
		Sleep(500);
		UVCommand2.UV_Shutter_PowerSet(model.UV_Power);// 95);//
		sLog.Format("	#2 UV POWER SET: %d", model.UV_Power);
		putListLog(sLog);
		iRtnFunction = 19000;
		break;

	case 19000:
		Task.AutoReday = 1;
		iRtnFunction = 19900;
		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);
		break;
	}
	return iRtnFunction;
}

int CPcbProcess2::RunProc_ProductLoading(int iUseStep)
{//! ÀÛ¾÷ÀÚ Á¦Ç° ÅõÀÔ ÀÚµ¿ Step		(10000 ~ 10999)
	int iRtnFunction = iUseStep;
	CString logStr = "", sBarCode = "";
	bool bChk = true;

	switch (iUseStep)
	{
	case 10000:
		Task.interlockLens = Task.interlockPcb = 0;
		iRtnFunction = 10010;
		break;
	case 10010:
		iRtnFunction = 10030;

		break;
	case 10030:
		iRtnFunction = 10050;
		break;
	case 10050:
		//LightControl.ctrlLedVolume(LIGHT_OC_6500K, 0);		// Align¸¸ Á¶¸í ON
		Task.PCBTaskTime = myTimer(true);

		iRtnFunction = 10060;
		break;

	case 10060:
		if (motor.IsStopAxis(Motor_Lens_X) && motor.IsStopAxis(Motor_Lens_Y) && motor.checkLensMotorPos(Wait_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 10070;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 30)
		{
			logStr.Format(_T("Lens x,y axis Á¤Áö È®ÀÎ ½ÇÆÐ[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10060;
		}
		break;
	case 10070:
#if (__MACHINE_MODEL == MACHINE______2ST)
		if (motor.IsStopAxis(Motor_Insp_X) && motor.checkInspMotorPos(Wait_Pos))
		{
			iRtnFunction = 10100;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 5000)
		{
			logStr.Format(_T("Insp x axis Á¤Áö È®ÀÎ ½ÇÆÐ[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10070;
		}
#endif
		break;
	case 10100://! Chip Á¤º¸ Draw
		if (LGIT_MODEL_INDEX == M2_OV9284_5M || LGIT_MODEL_INDEX == M2_VB1940_5M)
		{
			Dio.LensVacuumOn(true);
		}
		
		dwTickStartRun = false;
		theApp.MainDlg->func_ChipID_Draw();

		iRtnFunction = 10110;

		logStr.Format("		loading Step [%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("		Start ¹öÆ°À» ´­·¯ÁÖ¼¼¿ä[%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		theApp.MainDlg->m_btnStart.m_iStateBtn = 4;
		theApp.MainDlg->m_btnStart.Invalidate();
		break;
	case 10110://! Start P/B Button Push ´ë±â. 
		if (Dio.StartPBOnCheck(true, false) || dwTickStartRun == true)// || Start_Btn_On)	//ÇÁ·Î±×·¥¿¡ start ¹öÆ° Ãß°¡
		{
			dwTickStartRun = false;
			Task.interlockLens = 0;	//start bt
			iRtnFunction = 10165;
			MbufClear(vision.MilOptImage, 0);

			theApp.MainDlg->m_btnStart.m_iStateBtn = 0;
			theApp.MainDlg->m_btnStart.Invalidate();
			theApp.MainDlg->Start_Btn_On = false;

			if (Task.m_bOKLensPass == 1)   //Lens ³Ñ±è ¿Ï·á»óÅÂ
			{
				if (!askMsg("·»Áî ³Ñ±è»óÅÂÀÔ´Ï´Ù. \n°è¼Ó ÁøÇàÇÏ½Ã°Ú½À´Ï±î?") == IDOK)
				{
					iRtnFunction = 10000;
					break;
				}
			}
			logStr.Format("START BUTTON PRESS [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
		}
		break;
	case 10165://! Start P/B Button Push ´ë±â. 
		Dio.StartPBLampOn(false);
		sBarCode.Format("%s", Task.ChipID);
		if (sysData.m_iProductComp == 1)
		{
			logStr.Format("====%s ¿ÏÁ¦Ç° °Ë»ç start====", Task.ChipID);
		}
		else
		{
			logStr.Format("====%s AA start====", Task.ChipID);
		}
		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD);
		theApp.MainDlg->putListLog(logStr);
		iRtnFunction = 10170;
		break;
	case 10170://! Start P/B Button Push ´ë±â.
	{
		bool bflag = true;

		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;

		/*bflag = EpoxyTimeCheck(bEpoxyTimeChk);
		if(!bflag)
		{
		iRtnFunction = -10170;
		break;
		}*/
		MandoInspLog.func_InitData();

		Task.func_TactTimeInit();


		theApp.MainDlg->dispGrid();

		Task.m_timeChecker.Measure_Time(1);	//Start ¹öÆ° Å¬¸¯ ½Ã°£
		theApp.MainDlg->func_ChipID_Draw();
		MandoInspLog.bInspRes = true;
		vision.clearOverlay(CCD);
		int iSocketCount = 0;
		//logStr.Format("%d", sysData.m_Socket_Count);
		iSocketCount = atoi(logStr);
		iSocketCount++;
		sysData.m_Socket_Count = iSocketCount;
		sysData.Save();

		iRtnFunction = 10180;
	}
	break;
	case 10180:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z Wait À§Ä¡ ÀÌµ¿ ¿Ï·á [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10200;
		}
		else
		{
			logStr.Format("PCB Z Wait À§Ä¡ ÀÌµ¿ ¿Ï·á ½ÇÆÐ [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10180;
		}
		break;
	case 10200:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10202;
			logStr.Format("		Ready Step [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING745);	//LENSºÎ Tx, TyÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			logStr.Format(sLangChange);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = -10200;
		}
		break;

	case 10202:

		iRtnFunction = 10205;
		break;

	case 10205:
		if (sysData.m_iProductComp == 1)
		{
			iRtnFunction = 10250;
		}
		else
		{
			iRtnFunction = 10210;
		}
		break;

	case 10210:

		iRtnFunction = 10250;
		break;

	case 10250:
		iRtnFunction = 10400;
		break;
	case 10400:	//! ¹ÙÄÚµå Á¤º¸ Check
		sBarCode.Format("%s", Task.ChipID);
		g_ADOData.func_AATaskToRecordID(modelList.curModelName, Task.ChipID);

		//		if (sBarCode == "EMPTY")//¹ÙÄÚµåÁ¤º¸°¡ ¾øÀ» °æ¿ì È®ÀÎ Message
		//		{
		//			logStr.Format("¹ÙÄÚµå Á¤º¸°¡ ¾ø½À´Ï´Ù. \n [ %s ]¸íÀ¸·Î ÁøÇà ÇÏ½Ã°Ú½À´Ï±î?", Task.ChipID);
		//			if (askMsg(logStr) == IDOK)
		//			{
		//				sBarCode.Format("%s", Task.ChipID);
		//			}
		//			else
		//			{
		//				iRtnFunction = 10100;
		//				logStr.Format("		loading Step [%d]", iRtnFunction);
		//				putListLog(logStr);
		//				break;
		//			}
		//		}
		//		else
		//		{
		//			if (!g_ADOData.func_AATaskToRecordID(modelList.curModelName, Task.ChipID)) 
		//			{
		//				logStr.Format("		loading Step [%d]", iRtnFunction);
		//				putListLog(logStr);
		//#if (____AA_WAY == PCB_TILT_AA)
		//				/*if(sysData.m_iProductComp==1)
		//				{
		//					logStr.Format("ÀÏÄ¡ÇÏ´Â ¾ÆÀÌµð°¡ ¾ø½À´Ï´Ù.[%d]", iUseStep);
		//				}else
		//				{
		//					logStr.Format("DataBase »ý¼º ½ÇÆÐ.[%d]\n MS Office¸¦ ´Ý¾ÆÁÖ¼¼¿ä.", iUseStep);
		//				}
		//				errMsg2(Task.AutoFlag,logStr);
		//				iRtnFunction = -10400;
		//				break;*/
		//#endif		
		//
		//			}
		//		}


		iRtnFunction = 10440;

		break;
	case 10440:
		if (sysData.m_testRunMode == false)
		{
			pThread_TaskCCD = ::AfxBeginThread(Thread_TaskCCD, this);

		}
		iRtnFunction = 10500;
		break;
	case 10500:
		iRtnFunction = 10900;
		break;

	case 10900:
		Task.PcbOnStage = 100;
		Task.m_b_AA_Retry_Flag = false;

		logStr.Format("%d", Task.m_iPcbPickupNo);
		theApp.MainDlg->m_labelPickupNoPcb.SetText(logStr);
		theApp.MainDlg->m_labelPickupNoPcb.Invalidate();

		MandoInspLog.sBarcodeID.Format("%s", Task.ChipID);	//¸¸µµ Â÷·®¿ëCamera °Ë»ç Log ÀúÀå
		iLaser_Pos = 0;


		if (sysData.m_iIrChartUse == 1)
		{
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, 0);
			Sleep(100);
			LightControlSecond.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP2_CHART]);
		}
		else
		{
			LightControlSecond.ctrlLedVolume(LIGHT_TOP_CHART1, 0);
			Sleep(100);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP1_CHART]);
			//LightControl.ctrlLedVolume(LIGHT_TOP_CHART2, model.m_iLedValue[LEDDATA_TOP2_CHART]);
			//LightControl.ctrlLedVolume(LIGHT_TOP_CHART3, model.m_iLedValue[LEDDATA_TOP3_CHART]);
			//LightControl.ctrlLedVolume(LIGHT_TOP_CHART4, model.m_iLedValue[LEDDATA_TOP4_CHART]);
			//LightControl.ctrlLedVolume(LIGHT_LEFT_CHART, model.m_iLedValue[LEDDATA_CHART_L]);
			//LightControl.ctrlLedVolume(LIGHT_RIGHT_CHART, model.m_iLedValue[LEDDATA_CHART_R]);
		}




#if (____AA_WAY == PCB_TILT_AA)
		if (Task.m_bOKLensPass == 1)   //Lens ³Ñ±è ¿Ï·á»óÅÂ
		{
			logStr.Format("		Lens ±×¸³ ÁøÇà [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 15500;// 10250;
			break;
		}
#else
		if (sysData.m_iProductComp == 0)		//lens Thread µ¿ÀÛ ºÎºÐ
		{
			if (Task.m_bOKLensPass != 1)
			{
				Task.LensTask = 30000;	//Lens AA,, ·»Áî ·Îµå µ¿½Ã µ¿ÀÛ, °£¼· È®ÀÎÈÄ »ç¿ëÇÏ½Ã¿À 240115
			}
			else
			{
				Task.interlockLens = 1;		//Lens AA »ç¿ë
				logStr.Format("Lens ³Ñ±è ¿Ï·á  [%d]", iRtnFunction);
				theApp.MainDlg->putListLog(logStr);
			}
		}
#endif
		iRtnFunction = 11000;
		break;

	default:
		sLangChange.LoadStringA(IDS_STRING1007);	//PCB Á¦Ç° ÅõÀÔ Thread Step ¹øÈ£ ºñÁ¤»ó . 
		logStr.Format("PCB Á¦Ç° ÅõÀÔ Thread Step ¹øÈ£ ºñÁ¤»ó [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess2::RunProc_PCBOutsideAlign(int iUseStep)
{
	//! PCB ¿ÜºÎ Align Step(11000 ~ 14999)					//!! MIU_Initialize -> PCB Holder Align À§Ä¡
	int iRtnFunction = iUseStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;		//PCB ¿µ»ó È¹µæ Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	//ctrlSubDlg(MAIN_DLG);		//20161212  ccd start ¼öÁ¤
	theApp.MainDlg->m_iCurCamNo = 0;
	theApp.MainDlg->setCamDisplay(1, 0);
	theApp.MainDlg->changeMainBtnColor(MAIN_DLG);

	switch (iUseStep)
	{
	case 11000:
		/////////////////////////
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 11500;
		break;

	case 11500:
		Task.d_Align_offset_x[PCB_Holder_MARK] = 0;
		Task.d_Align_offset_y[PCB_Holder_MARK] = 0;
		Task.d_Align_offset_th[PCB_Holder_MARK] = 0;
		iRtnFunction = 12000;
		break;
	case 12000:
		iRtnFunction = 14100;		//holder align pass 240723
		break;
		break;
		break;
		break;
		break;
		break;
		break;
		break;
		break;
		break;
		if (Task.m_bOKLensPass == 1 || sysData.m_iAlignCamInspPass == 1)   //Lens ³Ñ±è ¿Ï·á»óÅÂ RunProc_PCBOutsideAlign
		{
			iRtnFunction = 14100;
			break;
		}

		//240109 ¹Ì¸® ÀÌµ¿
		if (motor.INSP_Motor_MoveX(Holder_Pos, 0.0, false))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Insp X Holder À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 13000;
		}
		else
		{
			logStr.Format("Insp X Holder À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12000;
		}
		break;
	case 13000:
		Task.m_iRetry_Opt = 0;
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		// Align¸¸ Á¶¸í ON
		
		if (motor.Pcb_Motor_Move(Holder_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb Holder À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 13300;
		}
		else
		{
			logStr.Format("Pcb Holder À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -13000;
		}
		break;

	case 13300:
		if (motor.PCB_Z_Motor_Move(Holder_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb Holder ZÀ§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			//151109 ¿µÁø ÀÓ½Ã µô·¹ÀÌ
			Sleep(300);
			iRtnFunction = 13500;
		}
		else
		{
			logStr.Format("Pcb Holder ZÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -13300;
		}
		break;
	case 13500:
#if (__MACHINE_MODEL == MACHINE______2ST)
		if (motor.IsStopAxis(Motor_Insp_X) && motor.checkInspMotorPos(Holder_Pos))
		{
			iRtnFunction = 13600;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 5000)
		{
			logStr.Format("Insp X HolderÀ§Ä¡ ÀÌµ¿ ÃÊ°ú..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -13600;
		}
#endif
		break;
	case 13600:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 13700;
		}
		break;
	case 13700: // lens Align

		if (Task.m_iRetry_Opt > iAlignRetry)
		{
			logStr.Format("PCB Holder Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]\n º¸Á¤ ¹ÌÀû¿ëÀ¸·Î ÁøÇàÇÏ½Ã°Ú½À´Ï±î?", Task.m_iRetry_Opt, iUseStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[PCB_Holder_MARK] = 0;
				Task.d_Align_offset_y[PCB_Holder_MARK] = 0;
				Task.d_Align_offset_th[PCB_Holder_MARK] = 0;
				iRtnFunction = 14100;
			}
			else
			{
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 13700;
				logStr.Format("PCB Holder Àç°Ë»ç ½ÃÀÛ[%d]", iUseStep);
				putListLog(logStr);
				//logStr.Format("PCB Holder Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iUseStep);
				//errMsg2(Task.AutoFlag,logStr);
			}

			break;
		}

		offsetX = offsetY = offsetTh = 0.0;
		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, PCB_Holder_MARK, false, offsetX, offsetY, offsetTh);
		offsetTh = 0.0;
		saveInspImage(PCB_IMAGE_SAVE, Task.m_iRetry_Opt);

		Task.m_iRetry_Opt++;
		offsetTh = 0;//holder x,yÃà¸¸
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[PCB_Holder_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Holder_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Holder_MARK] += offsetTh;

			logStr.Format("Pcb Holder : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]);
			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2 - 1, offsetX, offsetY, offsetTh);
			if (iRtnVal == 1)
			{
				iRtnFunction = 14000;				// º¸Á¤ ÀÌµ¿
			}
			else if (iRtnVal == 2)
			{
				sLangChange.LoadStringA(IDS_STRING654);	//Lens Align [%d] ¿Ï·á [%d]
				logStr.Format("		Pcb Holder [%d] ¿Ï·á [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);

				theApp.MainDlg->InstantMarkDelete(PCB_Holder_MARK);			//ÀÓ½Ã µî·Ï ¸¶Å© »èÁ¦

				logStr.Format("		Pcb Holder Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]);//0.000);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 14100;				// ¿Ï·á
			}
			else
			{
				logStr.Format("Pcb Holder [%d] º¸Á¤ ¹üÀ§ ÃÊ°ú[%d]", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				theApp.MainDlg->InstantMarkDelete(PCB_Holder_MARK);
				iRtnFunction = -13700;				// NG
			}
		}
		else
		{
			logStr.Format("Pcb Holder [%d] ¸¶Å© ÀÎ½Ä ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);
			Sleep(300);
			iRtnFunction = 13700;					//  Àç°Ë»ç 
		}
		break;

	case 14000://!! º¸Á¤·® ÀÌµ¿.- Retry
		if (motor.Pcb_Holder_Align_Move(Holder_Pos, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 13500;
		}
		else
		{
			logStr.Format("Pcb Holder º¸Á¤ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -14000;
		}
		break;
	case 14100:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(" PCB ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -14100;
			break;

		}
		iRtnFunction = 14200;
		break;
	case 14200:
		if (!motor.INSP_Motor_MoveX(Wait_Pos))
		{
			logStr.Format(" Insp XÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -14200;
			break;
		}
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 15000;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 26000;

#endif
		break;
	default:
		logStr.Format("PCB Á¦Ç° ÅõÀÔ Thread Step ¹øÈ£ ºñÁ¤»ó . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	return iRtnFunction;
}

int	CPcbProcess2::RunProc_SensorAlign(int iUseStep)
{
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = _T("");
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ¿µ»ó È¹µæ Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ÃøÁ¤Àü Delay
	switch (iUseStep)
	{
	case 26000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("		PCB ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -26000;
			break;
		}

		Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_th[PCB_Chip_MARK] = 0;

		iRtnFunction = 26050;
		break;
	case 26050:
		if (sysData.m_iAlignCamInspPass == 1)	//RunProc_SensorAlign
		{
			Task.m_iRetry_Opt = 0;
			iRtnFunction = 26629;
			break;
		}
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 26600;
		break;

	case 26600:
		if (motor.INSP_Motor_MoveX(Align_Pos))
		{
			logStr.Format("Insp X Ãà PCB Sensor Align À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 26610;
		}
		else
		{
			logStr.Format("Insp X Ãà PCB Sensor Align À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -26600;
		}
		break;

	case 26610:
		//ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		//SetDigReference(PCB_Chip_MARK);
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);		// Align¸¸ Á¶¸í ON

		if (motor.Pcb_Motor_Move(Align_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Align_Pos))
			{
				logStr.Format("PCB Sensor Align À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
				putListLog(logStr);
				
				Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
				iRtnFunction = 26615;
				break;
			}
			else
			{
				logStr.Format("PCB Sensor Align Z À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -26610;
				break;
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align X,YÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -26610;
		}
		break;
	case 26615:	
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 26616;
		break;
	case 26616:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 26620;
		}
		break;
	case 26620:
		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB Align(*SensorAlign*) Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]\n º¸Á¤ ¹ÌÀû¿ëÀ¸·Î ÁøÇàÇÏ½Ã°Ú½À´Ï±î?", Task.m_iRetry_Opt, iUseStep);
			Task.d_Align_offset_x[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_y[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_th[PCB_Chip_MARK] = 0.0;
			Task.m_iRetry_Opt = 0;
			if (askMsg(logStr) == IDOK)
			{
				iRtnFunction = 26629;
				break;
			}
			else
			{
				logStr.Format("PCB Sensor Align ½ÇÆÐ [%d]", iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -26620;
				/*iRtnFunction = 26620;
				logStr.Format("PCB Align Àç°Ë»ç ½ÃÀÛ[%d]", iUseStep);
				putListLog(logStr);
				break;*/
			}
			break;
		}

		offsetX = offsetY = offsetTh = 0.0;

		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, PCB_Chip_MARK, false, offsetX, offsetY, offsetTh);
		saveInspImage(CHIP_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;

		//offsetTh=0;
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[PCB_Chip_MARK] += offsetX; 
			Task.d_Align_offset_y[PCB_Chip_MARK] += offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] += offsetTh;


			Task.dPcbAlignTh = Task.d_Align_offset_th[PCB_Chip_MARK];

			logStr.Format("PCB Sensor Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2 - 1, offsetX, offsetY, offsetTh);

			if (iRtnVal == 1)
			{
				iRtnFunction = 26625;				// º¸Á¤ ÀÌµ¿
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		PCB Sensor Align [%d] ¿Ï·á [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);			//ÀÓ½Ã µî·Ï ¸¶Å© »èÁ¦

				logStr.Format("		PCB Sensor Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;

				Task.m_timeChecker.Measure_Time(4);

				iRtnFunction = 26629;		// ¿Ï·á
				LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);
				break;
			}
			else
			{
				logStr.Format("PCB Sensor Align [%d] º¸Á¤ ¹üÀ§ ÃÊ°ú", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -26620;				// NG
				break;
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align [%d] ¸¶Å© ÀÎ½Ä ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);
			Sleep(300);
			iRtnFunction = 26620;				//  Àç°Ë»ç 
		}
		break;
	case 26625://- Sensor Align Retry
		if (motor.Pcb_Holder_Align_Move(Align_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 26615;
		}
		else
		{
			logStr.Format("PCB Sensor Align º¸Á¤ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -26625;
		}
		break;
	case 26629:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			logStr.Format(sLangChange, iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -26629;
			break;
		}
		iRtnFunction = 26990;
		break;
	case 26990:				
		iRtnFunction = 26991;
		break;
	case 26991://! Lens-ZÃà ´ë±â À§Ä¡ ÀÌµ¿
#if (____AA_WAY == PCB_TILT_AA)
		if (sysData.m_iEpoxyLaserInspPass == 1)
		{
			iRtnFunction = 30000;	//µµÆ÷ÀÌµ¿
		}
		else
		{
			iRtnFunction = 27000;	//LaserÃøÁ¤ ->Align -> µµÆ÷
		}
#elif (____AA_WAY == LENS_TILT_AA)
		if (sysData.m_iEpoxyLaserInspPass == 1)
		{
			iRtnFunction = 30000;//Laser ÃøÁ¤ Pass
		}
		else
		{
			iRtnFunction = 27000;
		}
#endif
		break;
	default:
		logStr.Format("Lens ¿ÜºÎ Align Thread Step ¹øÈ£ ºñÁ¤»ó [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int	CPcbProcess2::RunProc_LaserMeasure(int iUseStep)
{
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ¿µ»ó È¹µæ Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ÃøÁ¤Àü Delay
	switch (iUseStep)
	{
	case 27000:
		Task.oldLaserTx = 0.1;
		Task.oldLaserTy = 0.1;
		iLaser_Pos = 0;// 4;//4¹øºÎÅÍ°¡ laser out point
		LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align¸¸ Á¶¸í ON


		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("[¿îÀü ÁØºñ] PCB ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -27000;
			break;
		}
		LightControlthird.ctrlLedVolume(LIGHT_OC, 0);
		//if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		//{
		//	iRtnFunction = 30000;	//-- µµÆ÷ Pass Mode ÀÌ°Å³ª, µµÆ÷ ¿Ï·á ÀÏ °æ¿ì
		//	break;
		//}
		MandoInspLog.dLaserTestTilt[0] = 0.0;
		MandoInspLog.dLaserTestTilt[1] = 0.0;
		iRtnFunction = 27100;// 27350;//<-ÀÓ½Ã ÆÐ½º   Original-> 27100; 
		break;

	case 27100:
		if (motor.Pcb_Motor_Move_Laser(iLaser_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Laser º¯À§ ÃøÁ¤ ¿ÜºÎ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27150;
		}
		else
		{
			logStr.Format("Laser º¯À§ ÃøÁ¤ ¿ÜºÎ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27100;
		}
		break;
	case 27150:				//IM
		if (motor.INSP_Motor_MoveX(Laser_Pcb_Pos))
		{
			logStr.Format("Insp X Ãà PCB Laser_Pcb_Pos ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27151;
			break;
		}
		else
		{
			logStr.Format("Insp X Ãà PCB Laser_Pcb_Pos ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27150;
			break;
		}
		break;
	case 27151:
		if (iLaser_Pos == 0)
		{
			if (!motor.PCB_Z_Motor_Move(Laser_Pcb_Pos))
			{
				logStr.Format("PCB Laser_Pcb_Pos Z ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -27151;
				break;
			}
			else
			{
				logStr.Format("PCB Laser_Pcb_Pos Z ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
				putListLog(logStr);
			}
		}
		iRtnFunction = 27200;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 27200:
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			Sleep(200);
			iRtnFunction = 27250;
		}
		break;
	case 27250:
		Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Sleep(100);
		logStr.Format("º¯À§¼¾¼­ %lf - %d À§Ä¡[%d]", Task.m_Laser_Point[iLaser_Pos], iLaser_Pos + 1, iUseStep);
		putListLog(logStr);
		LaserPos[iLaser_Pos].x = motor.GetEncoderPos(Motor_PCB_X);
		LaserPos[iLaser_Pos].y = motor.GetEncoderPos(Motor_PCB_Y);
		LaserValue[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];

		MandoInspLog.dLaserTestPoint[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];
		
		if (Task.m_Laser_Point[iLaser_Pos] != 0)
		{
			if (iLaser_Pos == 3)
			{
				if (theApp.MainDlg->func_Check_LaserValueErr(LaserValue) == false)
				{
					logStr.Format("Laser ÃøÁ¤°ªÀÌ»ó: %.04lf, %.04lf, %.04lf, %.04lf [%d]", LaserValue[0], LaserValue[1], LaserValue[2], LaserValue[3], iUseStep);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -27250;
					break;
				}

				if (theApp.MainDlg->_calcLaserTilt(LaserPos, LaserValue, Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]))
				{
					Task.m_dataOffset_x[0] = Task.d_Align_offset_xt[PCB_Chip_MARK];
					Task.m_dataOffset_y[0] = Task.d_Align_offset_yt[PCB_Chip_MARK];
					logStr.Format("	º¸Á¤·® Tx: %.04lf, Ty: %.04lf", Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]);
					putListLog(logStr);

					int Rnd = theApp.MainDlg->TiltAlignLimitCheck(Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]);
					if (Rnd == 2)
					{
						if (Task.m_dataOffset_x[0] == 0 || Task.m_dataOffset_y[0] == 0)
						{
							logStr.Format("Laser º¯À§ Â÷ °ª ÀÌ»ó ¹ß»ý..Xt : %lf, Yt : %lf", Task.m_dataOffset_x[0], Task.m_dataOffset_y[0]);
							//errMsg2(Task.AutoFlag, logStr);
							//iRtnFunction = -27250;
							//break;
						}
						if (!g_ADOData.func_AATaskToRecordLaser(Task.ChipID, Task.m_dataOffset_x[0], Task.m_dataOffset_y[0], Task.m_Laser_Point))
						{
							logStr.Format("DataBase Laser º¯À§ ÃøÁ¤ Data ±â·Ï ½ÇÆÐ.[%d]\n MS Office¸¦ ´Ý¾ÆÁÖ¼¼¿ä.", iUseStep);
							errMsg2(Task.AutoFlag, logStr);
							iRtnFunction = -27250;
							break;
						}
						Task.m_PcbLaserAfterTx = motor.GetCommandPos(Motor_PCB_Xt);
						Task.m_PcbLaserAfterTy = motor.GetCommandPos(Motor_PCB_Yt);

						logStr.Format("	PCB Laser tx:%lf , ty:%lf [%d]", model.axis[Motor_PCB_Xt].pos[Laser_Pcb_Pos], model.axis[Motor_PCB_Yt].pos[Laser_Pcb_Pos], iUseStep);
						putListLog(logStr);
						logStr.Format("	PCB Laser After tx:%lf , ty:%lf [%d]", Task.m_PcbLaserAfterTx, Task.m_PcbLaserAfterTy, iUseStep);
						putListLog(logStr);
						iRtnFunction = 27310;//->Laser½Ç¸°´õ»ó½Â, 27350;->PCB WaitPos
						iLaser_Pos++;
					}
					else if (Rnd == 1)
					{
						iRtnFunction = 27300;
					}
					else
					{
						logStr.Format("Laser Tilt  º¸Á¤°ª Limit¸¦ ÃÊ°ú ÇÏ¿´½À´Ï´Ù.[%d]", iUseStep);
						errMsg2(Task.AutoFlag, logStr);
						iRtnFunction = -27250;
					}
				}
			}
			else
			{
				iRtnFunction = 27100;
				iLaser_Pos++;
			}
		}
		else
		{
			iRtnFunction = 27250;
		}
		break;
	case 27300:
		MandoInspLog.dLaserTestTilt[0] += Task.d_Align_offset_xt[PCB_Chip_MARK];
		MandoInspLog.dLaserTestTilt[1] += Task.d_Align_offset_yt[PCB_Chip_MARK];
		if (motor.Pcb_Move_Tilt(Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]))
		{
			iRtnFunction = 27100;
			logStr.Format("PCB Xt : %lf, Yt : %lf [%d]", Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK], iUseStep);
			putListLog(logStr);
			iLaser_Pos = 0;
		}
		else
		{
			logStr.Format("PCB tx,tyÃà  À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27300;
		}
		break;
	case 27310:
		logStr.Format("PCB Laser End Xt : %lf, Yt : %lf [%d]", MandoInspLog.dLaserTestTilt[0], MandoInspLog.dLaserTestTilt[1], iUseStep);
		putListLog(logStr); 

		iLaser_Pos = 0;
		if (LGIT_MODEL_INDEX == M2_FF_MODULE)
		{
			iRtnFunction = 27315;
		}
		else
		{
			iRtnFunction = 27350;		//jump
		}
		Task.PCBTaskTime = myTimer(true);
		break;
	case 27315:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("[¿îÀü ÁØºñ] PCB ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -27315;
			break;
		}
		if (LGIT_MODEL_INDEX == M2_FF_MODULE)
		{
			iRtnFunction = 27320;		//ÇÑ¹ø´õ ´Ù¸¥ À§Ä¡ ·¹ÀÌÀú Æ÷ÀÎÆ® ÃøÁ¤¸¸
		}
		else
		{
			iRtnFunction = 27340;
		}
		break;
	case 27320:
		if (motor.Pcb_Motor_Move_ComLaser(iLaser_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("Com Laser º¯À§ ÃøÁ¤ ¿ÜºÎ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27325;

			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			logStr.Format("Com Laser º¯À§ ÃøÁ¤ ¿ÜºÎ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27320;
		}
		
		
		break;
	case 27325:
		if (iLaser_Pos == 0)
		{
			if (!motor.PCB_Z_Motor_Move(Com_Laser_Pos))
			{
				logStr.Format("PCB Com Laser_Pcb_Pos Z ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -27325;
				break;
			}
			else
			{
				logStr.Format("PCB Laser_Pcb_Pos Z ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
				putListLog(logStr);
			}
		}
		iRtnFunction = 27330;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 27330:
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			Sleep(200);
			iRtnFunction = 27332;
		}
		
		break;
	case 27332:
		Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Sleep(100);
		logStr.Format("Side º¯À§¼¾¼­ %lf - %d À§Ä¡[%d]", Task.m_Laser_Point[iLaser_Pos], iLaser_Pos + 1, iUseStep);
		putListLog(logStr);

		MandoInspLog.dLaserTestComPoint[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];
		
		if (Task.m_Laser_Point[iLaser_Pos] != 0)
		{
			if (iLaser_Pos == 3)
			{
				iRtnFunction = 27340;
				break;
			}
			else
			{
				iRtnFunction = 27320;
				iLaser_Pos++;
				break;
			}
		}
		else
		{
			iRtnFunction = 27332;
		}
		break;
	case 27340:
		iRtnFunction = 27350;
		break;
	case 27350:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27351;
		}
		else
		{
			logStr.Format("PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27350;
		}

		break;
	case 27351:
		//if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		//{
		//	iRtnFunction = 31000;	//-- µµÆ÷ Pass Mode ÀÌ°Å³ª, µµÆ÷ ¿Ï·á ÀÏ °æ¿ì
		//}
		//else
		//{
		//	iRtnFunction = 30020;
		//}
		if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		{
			if (motor.INSP_Motor_MoveX(Wait_Pos))		//µµÆ÷ÇÏ¸é ´ë±â·Î °¥ ÇÊ¿ä ¾ø´Ù.
			{
				logStr.Format("Insp XÃà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
				putListLog(logStr);
				iRtnFunction = 27600;
			}
			else
			{
				logStr.Format("Insp XÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -27351;
			}
		}
		else
		{
			if (motor.INSP_Motor_MoveX(Dispense_Pos))		//µµÆ÷ÇÏ¸é ´ë±â·Î °¥ ÇÊ¿ä ¾ø´Ù.
			{
				logStr.Format("Insp XÃà µµÆ÷ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
				putListLog(logStr);
				iRtnFunction = 27600;
			}
			else
			{
				logStr.Format("Insp XÃà µµÆ÷ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -27351;
			}
		}
		break;
	case 27600:

		iRtnFunction = 27650;
		break;
	case 27650:
		offsetX = offsetY = offsetTh = 0.0;
		iRtnFunction = 27750;
		break;
	case 27750:
		iRtnFunction = 27960;		// ¿Ï·á ÀÓ½Ã
									//
									//
									//
		break;





		Task.m_iRetry_Opt = 0;
		iRtnFunction = 27800;
		break;
	case 27800:
		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);		// Align¸¸ Á¶¸í ON

		if (motor.Pcb_Motor_Move(Align_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Align_Pos))
			{
				logStr.Format("PCB Sensor Align À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
				putListLog(logStr);
				Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
				iRtnFunction = 27850;
				Task.PCBTaskTime = myTimer(true);
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27960;
		}
		break;
	case 27850:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 27900;
		}
		break;
	case 27900:
		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB Align(*Laser*) Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]\n º¸Á¤ ¹ÌÀû¿ëÀ¸·Î ÁøÇàÇÏ½Ã°Ú½À´Ï±î?", Task.m_iRetry_Opt, iUseStep);
			Task.m_iRetry_Opt = 0;
			Task.d_Align_offset_x[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_y[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_th[PCB_Chip_MARK] = 0.0;
			if (askMsg(logStr) == IDOK)
			{
				iRtnFunction = 27960;
				logStr.Format("PCB Align ¹Ìº¸Á¤ ÁøÇà[%d]", iUseStep);
				putListLog(logStr);
			}
			else
			{
				iRtnFunction = 27900;
				logStr.Format("PCB Align Àç°Ë»ç ½ÃÀÛ[%d]", iUseStep);
				putListLog(logStr);
				Sleep(500);
			}
			break;
		}

		offsetX = offsetY = offsetTh = 0.0;

		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, PCB_Chip_MARK, false, offsetX, offsetY, offsetTh);

		saveInspImage(CHIP_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;
		//offsetTh=0;
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[PCB_Chip_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Chip_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] -= offsetTh;

			Task.dPcbAlignTh = Task.d_Align_offset_th[PCB_Chip_MARK];
			logStr.Format("PCB Sensor Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2 - 1, offsetX, offsetY, offsetTh);

			if (iRtnVal == 1)
			{
				iRtnFunction = 27950;				// º¸Á¤ ÀÌµ¿
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		PCB Sensor Align [%d] ¿Ï·á [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);
				logStr.Format("		PCB Sensor Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;

				Task.m_timeChecker.Measure_Time(4);	//Laser º¯À§ ÃøÁ¤ ½ÃÀÛ ½Ã°£


				iRtnFunction = 27960;		// ¿Ï·á
				LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align¸¸ Á¶¸í ON 
				LightControlthird.ctrlLedVolume(LIGHT_OC, 0);		// Align¸¸ Á¶¸í ON
			}
			else
			{
				logStr.Format("PCB Sensor Align [%d] º¸Á¤ ¹üÀ§ ÃÊ°ú", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				iRtnFunction = -27900;				// NG
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align(*Laser*) [%d] ¸¶Å© ÀÎ½Ä ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);
			Sleep(500);
			iRtnFunction = 27900;				//  Àç°Ë»ç 
		}
		break;
	case 27950:
		if (motor.Pcb_Holder_Align_Move(Align_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 27850;
		}
		else
		{
			logStr.Format("PCB Sensor Align º¸Á¤ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27950;
		}
		break;
	case 27960:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCBºÎ ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27960;
			break;
		}
		else
		{
			logStr.Format("PCBºÎ ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27970;
		}

		break;
	case 27970:
		iRtnFunction = 27990;
	case 27990:
		iRtnFunction = 30000;
		break;
	default:
		logStr.Format("Pcb Thread Step ¹øÈ£ ºñÁ¤»ó .[%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	return iRtnFunction;
}

int CPcbProcess2::RunProc_EpoxyNewResing(int iUseStep)
{//! Epoxy µ¿ÀÛ ¹× µµÆ÷ °Ë»ç Step(17000 ~ 17999)
	int iRtnFunction = iUseStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;		//PCB ¿µ»ó È¹µæ Delay
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ÃøÁ¤Àü Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;

	switch (iUseStep)
	{
	case 30000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1012);	//PCB_ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ.
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = -30000;
			break;
		}
		logStr.Format("PCB Z Wait À§Ä¡ ÀÌµ¿ ¿Ï·á [%d]", iUseStep);
		putListLog(logStr);

		if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		{
			iRtnFunction = 34100;// 31000;	//-- µµÆ÷ Pass Mode ÀÌ°Å³ª, µµÆ÷ ¿Ï·á ÀÏ °æ¿ì
		}
		else
		{
			iRtnFunction = 30020;
		}
		break;
	case 30020:
		iRtnFunction = 30030;
		break;
	case 30030:  //µµÆ÷ À§Ä¡ ÀÌµ¿
		LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align Á¶¸í OFF
		if (motor.Pcb_Motor_Move(Dispense_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING935);	//PCB Dispense º¸Á¤ À§Ä¡ ÀÌµ¿ ¿Ï·á-º¸Á¤·® X:%.03lf,Y:%.03lf,T:%.03lf
			logStr.Format(sLangChange, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			putListLog(logStr);
			iRtnFunction = 30040;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING938);	//PCB Dispense À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%
			logStr.Format(sLangChange + _T("d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30030;
		}
		break;
	case 30040:				//IM
		if (motor.INSP_Motor_MoveX(Dispense_Pos))
		{
			iRtnFunction = 30045;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1499);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Dispense_Pos], iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30040;
		}
		break;
	case 30045: //Dispenser-z Ãà »ó½Â
		if (motor.PCB_Z_Motor_Move(Dispense_Pos))
		{
			iRtnFunction = 30050;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING977);	//PCB Z Dispense À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30045;
		}
		break;
	case 30050: // µð½ºÆæ½º µ¿ÀÛ

		if (sysData.nEpoxyIndex == CRICLE_EPOXY)
		{
			bChk = motor.func_Epoxy_CircleDraw();
		}
		else if (sysData.nEpoxyIndex == RECT_EPOXY)
		{
			bChk = motor.func_Epoxy_Draw();
		}
		else if (sysData.nEpoxyIndex == POLYGON_EPOXY)
		{
			motor.func_Epoxy_Rect_Circle_Draw();
		}
		else if (sysData.nEpoxyIndex == POINT_EPOXY)
		{
			//point Çü
			logStr.Format("PCB ¼³Á¤ ¹ÌÁöÁ¤ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30050;
			break;
		}

		if (bChk)
		{
			Task.m_bOkDispense = 1;

			logStr.Format("PCB Dispense ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);

			iRtnFunction = 30070;
		}
		else
		{
			logStr.Format("PCB Dispense ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30050;
		}
		break;
	case 30070:
		if (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y))
		{
			LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);//µµÆ÷ °Ë»çÀü ¾ó¶óÀÎ¸¶Å©¿ë ¶óÀÌÆ®
			iRtnFunction = 30100;
		}
		break;
	case 30100:
		iRtnFunction = 31000;
		break;
	case 31000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			logStr.Format(sLangChange, iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -31000;
			break;
		}
		else
		{
			logStr.Format("PCB Z ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 32000;// 34000;
			break;
		}

		/*if( sysData.m_iDispenseInspPass == 0 )
		{
		iRtnFunction = 32000;

		}else
		{
		iRtnFunction = 34000;
		}*/
		break;
	case 32000:				//IM
		if (motor.INSP_Motor_MoveX(Holder_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1498);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Holder_Pos], iUseStep);
			iRtnFunction = 32100;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1499);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Holder_Pos], iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32000;
		}
		break;
	case 32100: // µµÆ÷°Ë»ç ÀÌµ¿
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		// Align¸¸ Á¶¸í ON
		if (motor.Pcb_Motor_Move(Holder_Pos))
		{
			iRtnFunction = 32500;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING989);	//PCB µµÆ÷°Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32100;
		}
		break;

	case 32500:// ÀÌµ¿ÈÄ, ÀÏÁ¤ ½Ã°£ Áö¿¬ ¹× ¸¶Å© È®ÀÎ
		if (motor.PCB_Z_Motor_Move(Holder_Pos))

		{
			Task.PCBTaskTime = myTimer(true);
			sLangChange.LoadStringA(IDS_STRING990);	//PCB µµÆ÷°Ë»ç À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = 33000;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING989);	//PCB µµÆ÷°Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32500;
		}
		break;
	case 33000: //µµÆ÷°Ë»ç zÃà ÀÌµ¿

		if ((myTimer(true) - Task.PCBTaskTime) > 1000 && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
#if 1
			vision.bmpImageSaveFn(CAM1, 0);
			iRtnFunction = 33500;
#else
			Sleep(300);
			int iMarkNo = 0;
			CString sTemp = "";

			vision.clearOverlay(CAM1);

			if (vision.getLiveMode())
			{
				vision.getSnapImage(CAM1);
				vision.setLiveMode(true);
			}

			double ep1 = 0.0;
			MappTimer(M_TIMER_RESET, &ep1);
			iMarkNo = vision.findMark(CAM1, PCB_Holder_MARK);

			double ep2 = 0.0;
			MappTimer(M_TIMER_READ, &ep2);
			sTemp.Format(" %.3lf sec", ep2 - ep1);
			vision.textlist[CAM1].addList((CAM_SIZE_X - 140), (CAM_SIZE_Y - 60), sTemp, M_COLOR_GREEN, 24, 10, "Arial");

			vision.drawOverlay(CAM1);

			if (iMarkNo != -1)
			{
#ifdef USE_GEOMETRIC
				MgraColor(M_DEFAULT, M_COLOR_GREEN);
				MmodDraw(M_DEFAULT, vision.ModResult[PCB_Holder_MARK][iMarkNo], vision.MilOverlayImage[CAM1], M_DRAW_BOX + M_DRAW_POSITION + M_DRAW_EDGES + M_DRAW_AXIS, M_DEFAULT, M_DEFAULT);

#endif
			}
			CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;
			pFrame->m_rBox.left = 0;
			pFrame->m_rBox.top = 0;
			pFrame->m_rBox.right = 0;
			pFrame->m_rBox.bottom = 0;
			iRtnFunction = 33500;
#endif
			
		}
		//LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_EPOXY_INSP]);
		break;
	case 33500: //µµÆ÷°Ë»ç 
		iRtnFunction = 34000;
#if 0
		Sleep(100);
		vision.clearOverlay(CAM1);
		bChk = theApp.MainDlg->_EpoxyFinddispense(CAM1);
		vision.drawOverlay(CAM1);
		if (!bChk)
		{
			putListLog("Epoxy µµÆ÷ °Ë»ç ½ÇÆÐ");

			logStr.Format("Epoxy µµÆ÷ °Ë»ç ½ÇÆÐ. \nÀÚµ¿ ¿îÀü ÁøÇà ÇÏ½Ã°Ú½À´Ï±î?");

			Dio.setAlarm(ALARM_ON);
			if (askMsg(logStr))
			{
				Task.PCBTaskTime = myTimer(true);
				Dio.setAlarm(ALARM_OFF);
				Dio.setAlarm(AUTO_RUN);
			}
			else
			{
				Dio.setAlarm(AUTO_STOP);
				logStr.Format("Epoxy µµÆ÷ °Ë»ç ½ÇÆÐ[%d]", iUseStep);
				iRtnFunction = -33500;
				break;
			}
		}
		Task.m_timeChecker.Measure_Time(3);	//µµÆ÷ ¿Ï·á ½Ã°£
		Task.m_dTime_Epoxy = Task.m_timeChecker.m_adTime[3] - Task.m_timeChecker.m_adTime[2];
		theApp.MainDlg->dispGrid();

		logStr.Format("PCB µµÆ÷°Ë»ç ¿Ï·á[%d]", iUseStep);
		putListLog(logStr);
#endif
		
		break;
	case 34000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1012);	//PCB_ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ.
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = -34000;
			break;
		}

		iRtnFunction = 34100;
		break;
	case 34100:
		if (!motor.INSP_Motor_MoveX(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1499);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Wait_Pos], iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -34000;
			break;
		}

#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 39000;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 35000;
#endif
		break;
	default:
		sLangChange.LoadStringA(IDS_STRING939);	//PCB Dispenser Thread Step ¹øÈ£ ºñÁ¤»ó .
		logStr.Format(sLangChange + _T(" [%d]"), iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess2::RunProc_InspAAPos(int iUseStep)
{
	//! Lens Pickup¿¡ Á¦Ç° ³Ñ±è Step	(18000 ~ 19999)

	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	switch (iUseStep)
	{
	case 39000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Wait Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39000;
			break;
		}

		iRtnFunction = 39050;
		break;
	case 39050:
		//240108 µ¿½Ã ÀÌµ¿ pcb º»µùÀ§Ä¡
		logStr.Format("PCB %s ÀÌµ¿ ¸í·É[%d] ", MotorPosName[Bonding_Pos], iUseStep);
		putListLog(logStr);

		if (!motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK], false))	//if (!motor.Pcb_Motor_MoveY(Bonding_Pos))
		{
			logStr.Format("PCB %s ÀÌµ¿ ¸í·É ½ÇÆÐ ", MotorPosName[Bonding_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -39050;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 39060;
	case 39060:

#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 41000;
#elif (____AA_WAY == LENS_TILT_AA)
		if (Task.interlockLens == 1 && Task.m_bOKLensPass == 1)
		{
			iRtnFunction = 41000;//// 40000;		240115
			break; 
		}
		if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 25)	//10ÃÊ ´ë±â
		{
			logStr.Format(_T("Lens Load È®ÀÎ ½ÇÆÐ[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39060;
			break;
		}
#endif	
		break;
		//

		//

		//
		//


		iRtnFunction = 39700;//aa¶§ defect Pass
		break;
	case 39100:
		LightControlthird.ctrlLedVolume(LIGHT_OC, model.m_iLedValue[LEDDATA_6500K]);
		if (!motor.Pcb_Motor_Move(OC_6500K_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("PCBºÎ ¸ðÅÍ 0C 6500K °Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -39100;
		}
		else
		{
			iRtnFunction = 39300;
		}
		break;
	case 39300:
		if (!motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			logStr.Format(_T("PCB Z axis 0C 6500K Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39300;
			break;
		}
		else
		{
			logStr.Format(_T("PCB Z axis 0C 6500K Pos Move Complete [%d]"), iUseStep);
			//errMsg2(Task.AutoFlag,logStr);
			iRtnFunction = 39350;
			break;
		}


	case 39350: //UVÀü OC Defect °Ë»ç 
		Sleep(100);
		MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);		//aa oc
		logStr.Format("MID_6500K_RAW SAVE OK [%d]", iUseStep);
		putListLog(logStr);
		Sleep(500);

		LightControlthird.ctrlLedVolume(LIGHT_OC, 0);
		iRtnFunction = 39400;
		break;

	case 39400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Defect Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39400;
			break;
		}
		else
		{
			logStr.Format(_T("PCB Z axis Defect Pos Move Complete [%d]"), iUseStep);
			//errMsg2(Task.AutoFlag,logStr);
			iRtnFunction = 39450;
			break;
		}

	case 39450:	//Dark °Ë»ç À§Ä¡ ÀÌµ¿
		iRtnFunction = 39500;
		//if(model.m_IsUseDarkDefectTest == 1)	// ¾Ï¸· Defect Picxel °Ë»ç À¯¹«
		//{
		//	iRtnFunction = 18102;				//¾Ï¸· °Ë»ç À§Ä¡ ÀÌµ¿
		//}
		//else
		//{
		//	iRtnFunction = 18115;				
		//}
		break;

	case 39500:	//Dark °Ë»ç À§Ä¡ ÀÌµ¿
		if (!motor.Pcb_Motor_Move(Dark_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING1022);	//PCBºÎ ¸ðÅÍ Dark °Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = -39500;
		}
		else
		{
			LightControlthird.ctrlLedVolume(LIGHT_OC, 0);
			LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);
			iRtnFunction = 39600;
		}
		break;

	case 39600:
		if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			logStr.Format(_T("PCB Z axis Dark Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39600;
		}
		else
		{
			logStr.Format(_T("PCB Z axis Dark Pos Move Complete [%d]"), iUseStep);
			//errMsg2(Task.AutoFlag,logStr);
			iRtnFunction = 39620;
		}
		break;

	case 39620:
		Sleep(500);
		MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);		//aa dark
		logStr.Format("LOW_LEVEL_RAW SAVE OK [%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 39650;
		break;
	case 39650:
		if (g_clPriInsp.func_Insp_Defect(MIU.vDefectMidBuffer_6500K, MIU.vDefectLowBuffer, false) == false)
		{
			logStr.Format("[AUTO] Defect °Ë»ç ½ÇÆÐ [%d]", iUseStep);
			MandoInspLog.bInspRes = false;
		}

		logStr.Format("[AUTO] Defect °Ë»ç ¿Ï·á [%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 39700;
		break;

	case 39700:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Wait Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39700;
		}
		else
		{
			iRtnFunction = 39850;
		}
		break;
	case 39850:
		iRtnFunction = 39900;
		break;
	case 39900: // º»µù À§Ä¡ ÀÌµ¿
		iRtnFunction = 39999;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 39999:			//¿©±â±îÁö¿È 180706_1
						//if( func_MIU_ConnectLiveCheck() == false)		//MIU ¿µ»ó Check
						//{
						//	iRtnFunction = -39999;
						//	break;
						//}
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 41000;
#elif (____AA_WAY == LENS_TILT_AA)
		if (Task.interlockLens == 1 && Task.m_bOKLensPass == 1)
		{
			iRtnFunction = 41000;		//2È£±â ·»Áî´Â Lens Thread¿¡¼­ ·Îµå
			break;
		}
		if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 10)	//10ÃÊ ´ë±â
		{
			logStr.Format(_T("Lens Load È®ÀÎ ½ÇÆÐ[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39999;
			break;
		}

#endif		
		break;
	default:
		logStr.Format("Pcb Align µ¿ÀÛ Thread Step ¹øÈ£ ºñÁ¤»ó . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int CPcbProcess2::RunProc_Bonding_Pos_GO(int iUseStep)
{//! Lens Pickup¿¡ Á¦Ç° ³Ñ±è Step	(18000 ~ 19999)
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	switch (iUseStep)
	{
	case 41000:
		if (sysData.m_iIrChartUse == 1)
		{
			LightControl.ctrlLedVolume(0,0);
			Sleep(100);
			LightControlSecond.ctrlLedVolume(0, model.m_iLedValue[LEDDATA_TOP2_CHART]);
			//LightControlSecond.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP2_CHART]);
		}
		else
		{
			LightControlSecond.ctrlLedVolume(0, 0);
			Sleep(100);
			LightControl.ctrlLedVolume(0, model.m_iLedValue[LEDDATA_TOP1_CHART]);
			Sleep(100);
			LightControlFourth.ctrlLedVolume(0, model.m_iLedValue[(LEDDATA_CHART_R)]);
			Sleep(100);
			LightControlFifth.ctrlLedVolume(0, model.m_iLedValue[(LEDDATA_CHART_L)]);
		}
		iRtnFunction = 41020;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 41020:
		if (motor.IsStopAxis(Motor_Lens_X) && motor.checkLensMotorPos(Wait_Pos, 1))
		{
			if (motor.Lens_Motor_MoveY(Bonding_Pos, 0.0, false))
			{
				logStr.Format("Lens YÃà Bonding À§Ä¡ ÀÌµ¿ ¸í·É[%d]", iRtnFunction);
				putListLog(logStr);
				iRtnFunction = 41030;
				Task.PCBTaskTime = myTimer(true);
				break;
			}
			else
			{
				logStr.Format("Lens YÃà Bonding À§Ä¡ ÀÌµ¿ ¸í·É ½ÇÆÐ", MotorPosName[Bonding_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41020;
				break;
			}
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 5000)
		{
			logStr.Format(_T("Lens x axis ´ë±âÀ§Ä¡ È®ÀÎ ½ÇÆÐ[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41020;
		}
		break;
	case 41030:
		if ((motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y) && motor.IsStopAxis(Motor_PCB_TH)))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 41050;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 30)
		{
			logStr.Format(_T("PCB x,y,th axis Á¤Áö È®ÀÎ ½ÇÆÐ[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
		}
		break;
	case 41050:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Wait Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41050;
		}
		else
		{
			iRtnFunction = 41100;
		}
		break;
	case 41100:
		iRtnFunction = 41150;
		break;
	case 41150:
		iRtnFunction = 41200;

		break;
	case 41200:
		iRtnFunction = 41220;

		break;
	case 41220:
#if (____AA_WAY == PCB_TILT_AA)
		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING913);	//"PCB Bonding À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]"
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = 41250;
			if (!motor.Pcb_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("PCB tx,ty Bonding_Pos ÀÌµ¿ ½ÇÆÐ");
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41200;
				break;
			}
			break;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
			break;
		}
#elif	(____AA_WAY == LENS_TILT_AA)
		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("PCB Bonding À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 41250;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
		}
#endif
		break;
	case 41250:
		if (motor.IsStopAxis(Motor_Lens_Y) && motor.checkLensMotorPos(Bonding_Pos, 0))
		{
			iRtnFunction = 41270;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 20)
		{
			logStr.Format(_T("Lens y axis º»µù À§Ä¡ È®ÀÎ ½ÇÆÐ[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41250;
		}
		break;
	case 41270:

		if (motor.Lens_Motor_MoveXY(1, Bonding_Pos))
		{
#if (____AA_WAY == LENS_TILT_AA)
			if (!motor.Lens_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("Lens %s Tx,TyÀÌµ¿ ½ÇÆÐ", MotorPosName[Bonding_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41200;
				break;
			}

#endif
			logStr.Format("Lens Bonding À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iRtnFunction);
			putListLog(logStr);
			Task.interlockPcb = 1;
			iRtnFunction = 41300;
			break;
		}
		else
		{
			logStr.Format("Lens Bonding À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41250;
			break;
		}
		break;
	case 41300:
		iRtnFunction = 41310;

		break;
	case 41310:
		if (Task.interlockPcb == 1)		//pcbÆ¿Æ®ÀÏ °æ¿ì  ·»Áî ·¹ÀÌÀú,¾ó¶óÀÎ ÀÛ¾÷ ³¡³­ÈÄ º»µùÀ§Ä¡ ¿Ã¶§±îÁö ±â´Ù·Á¾ßµÅ¼­
		{
			iRtnFunction = 41350;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 50000)
		{
			logStr.Format("Lens Bonding À§Ä¡ ÀÌµ¿ ½Ã°£ ÃÊ°ú[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41310;
		}
		break;
	case 41350:
		iRtnFunction = 41400;

		break;
	case 41400:
		//if( func_MIU_ConnectLiveCheck() == false)		//MIU ¿µ»ó Check
		//{
		//	iRtnFunction = -41400;
		//	break;
		//}

		theApp.MainDlg->setCamDisplay(3, 1);
		iRtnFunction = 48000;

		break;
	case 48000:

		LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align¸¸ Á¶¸í ON

		logStr.Format("Loading End[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 49000;
		break;
		//
	default:
		logStr.Format("Pcb Align µ¿ÀÛ Thread Step ¹øÈ£ ºñÁ¤»ó . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int	CPcbProcess2::procAutoFocus(int iStep)
{
	int iRtn = 0;
	int iRtnFunction = 0;
	CString logStr = _T("");
	int iDelayTime = 0;

	bool limitCheckFlag = false;

	double dMovePos = 0.0;

	double dAutoZ = 0.0;

	if (iStep >= 60300 && iStep != 69999)
	{
		if (gMIUDevice.bMIUOpen == 0 || gMIUDevice.CurrentState != 4)
		{
			logStr.Format("CCD µ¿¿µ»ó Á¤Áö »óÅÂÀÔ´Ï´Ù.");
			errMsg2(Task.AutoFlag, logStr);
			return iStep;
		}
	}

	if (Task.m_bFlag_Fine_Move == false)
	{
		iDelayTime = model.strInfo_AF1.m_iDelayTime;
	}
	else
	{
		iDelayTime = model.strInfo_AF2.m_iDelayTime;
	}

	iRtnFunction = iStep;

	switch (iStep)
	{
	case 60000:
	{
		MbufClear(vision.MilOptImage, 0x00);
		MbufClear(vision.MilDefectImage, 0x00);

		Task.initSFRElem();
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, "-");
		theApp.MainDlg->autodispDlg->DrawGraph(0);
		theApp.MainDlg->autodispDlg->DrawGraph(1);


		MandoInspLog.bInspRes = true;	//60000 ÃÊ±âÈ­
		Task.m_timeChecker.Measure_Time(6);	//AA ½ÃÀÛ ½Ã°£
		iRtnFunction = 60010;
		Task.PCBTaskTime = myTimer(true);
	}
	break;
	case  60010:
		if (bThreadCCDRun == false)		//	CCD ¿µ»ó ¾²·¹µå ¸ØÃß¸é ÁøÇà... step = 12000 Âü°í
		{
			iRtnFunction = 60015;
		}
		else
		{
			logStr.Format("CCD ¿µ»ó Open ÁøÇàÁßÀÔ´Ï´Ù[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60010;
		}
		break;
	case  60015:
		if (motor.LENS_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format("Lens Z Bonding À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 60020;

		}
		else
		{
			logStr.Format("Lens Z Bonding À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iStep);
			putListLog(logStr);
			iRtnFunction = -60015;
		}
		break;
	case 60020:
		if (!motor.PCB_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format("PCB ZÃà Bonding À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60020;
			break;
		}
		else
		{
			iRtnFunction = 60030;
			logStr.Format("PCB ZÃà Bonding À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iRtnFunction);
			putListLog(logStr);
		}

		break;
	case 60030:
	{
		double MotorPos[2];

		MotorPos[0] = fabs(motor.GetEncoderPos(theApp.MainDlg->TITLE_MOTOR_X) - model.axis[theApp.MainDlg->TITLE_MOTOR_X].pos[Bonding_Pos]);
		MotorPos[1] = fabs(motor.GetEncoderPos(theApp.MainDlg->TITLE_MOTOR_Y) - model.axis[theApp.MainDlg->TITLE_MOTOR_Y].pos[Bonding_Pos]);

		if ((MotorPos[0] < model.axis[theApp.MainDlg->TITLE_MOTOR_X].m_dLimit_Err && MotorPos[1] < model.axis[theApp.MainDlg->TITLE_MOTOR_Y].m_dLimit_Err))
		{
			logStr.Format("PCBºÎ x,y º»µù À§Ä¡¿¡ µµ´Þ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 60040;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
			break;
		}
		else
		{
			logStr.Format("Bonding À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60030;
			break;
		}
	}
	break;

	case 60040:		/* PCB Z¸ðÅÍ º»µùÀ§Ä¡ ÀÌµ¿ */
	{
		iRtn = theApp.MainDlg->_checkPcbMotor();
		if (iRtn > 0)
		{
			logStr.Format("Z ¸ðÅÍ º»µù À§Ä¡ ÀÌµ¿");
			theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, logStr);

			theApp.MainDlg->setCamDisplay(3, 1);
			double dCurPos = 0.0;
			double dMovePos = model.axis[Motor_Lens_Z].pos[Bonding_Pos];
			motor.MoveAxis(Motor_Lens_Z, ABS, dMovePos, sysData.fMotorSpeed[Motor_Lens_Z], sysData.fMotorAccTime[Motor_Lens_Z]);
			double ep = myTimer(true);

			while (1)
			{
				if (myTimer(true) - ep < 5000)
				{
					if (motor.IsStopAxis(Motor_Lens_Z))	// && motor.GetInposition(Motor_Lens_Z) )
					{
						dCurPos = motor.GetCommandPos(Motor_Lens_Z);

						if (fabs(dCurPos - dMovePos) <= 0.003)
						{
							Task.PCBTaskTime = myTimer(true);

							logStr.Format("Auto Z Pass");
							putListLog(logStr);
							iRtnFunction = 60900;
							logStr.Format("	AA Step [%d]", iRtnFunction);
							putListLog(logStr);
							break;
						}
					}
				}
				else
				{
					putListLog("Auto Focusing ñé LENS Z Motor ÀÌµ¿ ½Ã°£ ÃÊ°ú.");
					iRtnFunction = -60040;
					break;
				}
			}
		}
		else
		{
			putListLog("PCB °¡ º»µùÀ§Ä¡¿¡ µµ´ÞÇÏÁö ¸øÇß½À´Ï´Ù.");
			iRtnFunction = -60040;
			break;
		}
	}
	break;
	case 60070:		/* SFR °ªÀÌ ¿¬¼ÓÀ¸·Î ÇÏ¶ôÇÒ ¶§, Z¸ðÅÍ ÀÌµ¿ */
	{
		double dCurPos = 0.0;
		double dMovePos = model.axis[theApp.MainDlg->TITLE_MOTOR_Z].pos[Bonding_Pos] + model.m_dMove_Offset_Z;
		//¿¬¼ÓÀ¸·Î ÇÏ¶ô½Ã model.m_dMove_Offset_Z¸¸Å­ zÃàÀ» ¿òÁ÷ÀÎ´Ù 
		if (fabs(model.m_dMove_Offset_Z) > 0.2)
		{
			logStr.Format("zÃà ÀÌµ¿Ä¡ Limit ÃÊ°ú : Z : %.3lf", dMovePos);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60070;
			break;
		}
		motor.MoveAxis(theApp.MainDlg->TITLE_MOTOR_Z, ABS, dMovePos, sysData.fMotorSpeed[theApp.MainDlg->TITLE_MOTOR_Z], sysData.fMotorAccTime[theApp.MainDlg->TITLE_MOTOR_Z]);
		double ep = myTimer(true);
		Task.m_bFlag_Fine_Move = false;
		while (1)
		{
			if (myTimer(true) - ep< 5000)
			{
				if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z))
				{
					dCurPos = motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z);
					if (fabs(dCurPos - dMovePos) <= 0.003)
					{
						//240117 ÃÊ±âÈ­ ÄÚµå Ãß°¡
						Task.initSFRElem();
						theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, "-");
						theApp.MainDlg->autodispDlg->DrawGraph(0);
						theApp.MainDlg->autodispDlg->DrawGraph(1);


						Task.PCBTaskTime = myTimer(true);
						iRtnFunction = 60900;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
						break;
					}
				}
			}
			else
			{
				putListLog("Auto Focusing ñé Z Motor ÀÌµ¿ ½Ã°£ ÃÊ°ú.");
				iRtnFunction = -60070;
				break;
			}
		}
	}
	break;

	case 60100:
	{
		double dMoveDist = 0.0;
		if (!Task.m_bFlag_Fine_Move)
		{
			dMoveDist = model.strInfo_AF1.m_dMoveDistance;
			logStr.Format("Through Focusing");
			theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, logStr);
		}
		else
		{
			dMoveDist = model.strInfo_AF2.m_dMoveDistance;
			logStr.Format("Fine Focusing");
			theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, logStr);
		}

		iRtn = theApp.MainDlg->_moveZMotor(dMoveDist, model.strInfo_AF1.m_dMoveVel);		/* Z¸ðÅÍ Rough ÀÌµ¿ */
		if (iRtn > 0)
		{
			iRtnFunction = 61000;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			iRtnFunction = -60100;
			logStr.Format("	Z Motor ÀÌµ¿ ½Ã°£ ÃÊ°ú [%d]", iRtnFunction);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			break;
		}

	}
	break;
	case 60200:
	{
		if (Task.bFirstAA)
		{
			if (sysData.m_i1stAaEbable)	//1Â÷ AAÈÄ Á¾·á½Ã
			{
				iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);

				if (iRtn > 0)
				{
					Task.m_iCnt_Second_AA_Start = Task.m_iCnt_Step_AA_Total;
					Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
					Task.m_iCnt_Step_AA = 0;

					theApp.MainDlg->autodispDlg->DrawBarGraph();		// 60200  procAutoFocus
					MIU.func_Set_InspImageCopy(CHART_RAW, MIU.m_pFrameRawBuffer);			//1Â÷ AAÈÄ 
					Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);		//1Â÷ AAÈÄ Á¾·á
					if (Task.sfrResult)
					{
						logStr.Format("1Â÷ AA °á°ú Spec OK.");
						putListLog(logStr);
						iRtnFunction = 69999;
						break;
					}
				}
			}
			else
			{
				Task.m_iCnt_Second_AA_Start = Task.m_iCnt_Step_AA_Total;
				Task.m_iCnt_Step_AA = 0;
			}
		}
		double dMovePos = 0.0;
#if (____AA_WAY == PCB_TILT_AA)
		dMovePos = 1 * sysData.m_dMove_MaxPos_Fine;
#elif (____AA_WAY == LENS_TILT_AA)
		dMovePos = -1 * sysData.m_dMove_MaxPos_Fine;
#endif	


		iRtn = theApp.MainDlg->_moveZMotor(dMovePos, model.strInfo_AF1.m_dMoveVel);		/* Z¸ðÅÍ Rough ÀÌµ¿ */

		if (iRtn > 0)
		{
			iRtnFunction = 60900;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
		}
		else
			iRtnFunction = -60200;
	}

	break;

	case 60900:			/* ZÃà º»µù À§Ä¡ µµÂø ÈÄ */

		if ((myTimer(true) - Task.PCBTaskTime) > model.strInfo_Cam[CCD].m_iDelayTime)
		{
			iRtnFunction = 60910;
		}
		else
		{
			iRtnFunction = 60900;
		}

		break;

	case 60910:	// AAÀü Lens X/Y, PCB Theta º¸Á¤

		iRtn = theApp.MainDlg->_getMTF(SFR_FIRST/*SFR_FINAL*/);
		if (iRtn > 0)
		{
			if (!theApp.MainDlg->_calcImageAlignment())
			{
				logStr.Format("NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
				putListLog(logStr);
				iRtnFunction = -60910;
			}
			else
			{
				if (fabs(Task.m_dShift_IMG_X) <= sysData.m_dOcSpec.x * 2 && fabs(Task.m_dShift_IMG_Y) <= sysData.m_dOcSpec.y * 2 &&fabs(Task.m_dShift_IMG_TH) < 0.03)
				{
					logStr.Format("		 ¿µ»ó Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf",
						sysData.m_dOcSpec.x, sysData.m_dOcSpec.y,
						Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);
					iRtnFunction = 62000;
					break;
				}
				else
				{
					logStr.Format("		 ¿µ»ó Shift.  X %.3lf, Y %.3lf ,Spec(%.03lf, %.03lf)", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y, sysData.m_dOcSpec.x, sysData.m_dOcSpec.y);
					putListLog(logStr);
					iRtnFunction = 60920;
				}
			}
		}
		else
		{
			logStr.Format("SFR ¿µ»ó È¹µæ ½ÇÆÐ.");	//SFR ¿µ»ó È¹µæ ½ÇÆÐ.
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60910;
		}
		break;

	case 60920:
		if (!theApp.MainDlg->_MotorMove_IMG_Align())
		{
			iRtnFunction = -60920;
		}
		else
		{
			iRtnFunction = 60930;
		}
		break;

	case 60930:
		if (!theApp.MainDlg->_MotorMove_IMG_AlignTheta())
		{
			iRtnFunction = -60930;
		}
		else
		{
			Sleep(200);
			iRtnFunction = 60900;
		}
		break;

	case 61000:

		if ((myTimer(true) - Task.PCBTaskTime) > iDelayTime)
		{
			iRtnFunction = 62000;
		}

		break;

	case 62000:
	{
		iRtn = theApp.MainDlg->_getMTF((Task.bFirstAA == true) ? SFR_FINAL : SFR_FIRST);		//¼Óµµ¶§¹®¿¡ sfr ·Î±× ¼û±â±â
		logStr.Format("Step :[%d] SFR [%.03f] [Z %.03f]", Task.m_iCnt_Step_AA, Task.SFR.fSfrN4[Task.m_iCnt_Step_AA][0], motor.GetEncoderPos((theApp.MainDlg->TITLE_MOTOR_Z)));
		putListLog(logStr);
		if (iRtn>0)
		{
			Task.m_iCnt_Step_AA++;		/* Auto Focus rough ÃøÁ¤ È½¼ö Áõ°¡ */

			if (Task.bFirstAA)
			{
				theApp.MainDlg->autodispDlg->DrawGraph(1);		/* Z ¸ðÅÍ À§Ä¡º° SFR µ¥ÀÌÅÍ ±×·¡ÇÁ ±×¸®±â */
			}
			else
			{
				theApp.MainDlg->autodispDlg->DrawGraph(0);		/* Z ¸ðÅÍ À§Ä¡º° SFR µ¥ÀÌÅÍ ±×·¡ÇÁ ±×¸®±â */
			}

			if (!Task.bFirstAA == true)
			{
				if (Task.m_iCnt_Step_AA == model.m_iCnt_Check_SFR)
				{
					iRtnFunction = 63000;
				}
				else if (Task.m_bFlag_Decrease_SFR == true && Task.m_iCnt_Step_AA == model.m_iCnt_Check_SFR * 2)
				{
					iRtnFunction = 63000;
				}
				else
				{
					iRtnFunction = 64000;
				}
			}
			else
			{
				iRtnFunction = 64000;
			}
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			iRtnFunction = -63000;
		}
	}

	break;

	case 63000:		/* ÃÊ±â 3°³ µ¥ÀÌÅÍ È®ÀÎ ÈÄ °¨¼Ò ÁßÀÌ¸é ZÃà ¸ðÅÍ ÀÌµ¿ */
	{
		iRtn = theApp.MainDlg->_checkDecreaseSFR();
		if (iRtn > 0)
		{
			iRtnFunction = 64000;
		}
		else
		{
			Task.m_bFlag_Decrease_SFR = true;
			theApp.MainDlg->autodispDlg->m_iCur_Index_Through = Task.m_iCnt_Step_AA_Total;

			//autodispDlg->DrawGraph(0);			//------>20180607_1
			//autodispDlg->DrawGraph(1);
			iRtnFunction = 60070;
		}
	}
	break;

	case 64000:
	{
		iRtn = 1;
		int iRtn2 = 1;

		iRtn = theApp.MainDlg->_checkMaxSfrPos(0);		/* MAX °ª Ã£±â */

		theApp.MainDlg->ccdDlg->SetAlignData_Tilt(Task.SFR.dTilt_X, Task.SFR.dTilt_Y);

		if (iRtn>0)			// && iRtn2>0)
		{

			int mDixTx = 1;
			int mDixTy = 1;
			if (model.Tilt_Diretion[2] < 0)
			{
				mDixTx = -1;
			}
			if (model.Tilt_Diretion[3] < 0)
			{
				mDixTy = -1;
			}

			if (sysData.m_iDicChange == 0)										//IM
			{
				Task.SFR.dTilt_X = theApp.MainDlg->_calcTiltX() *  mDixTx;
				Task.SFR.dTilt_Y = theApp.MainDlg->_calcTiltY() *  mDixTy;
			}
			else
			{
				Task.SFR.dTilt_X = theApp.MainDlg->_calcTiltY() *  mDixTx; ;// mDixTy;
				Task.SFR.dTilt_Y = theApp.MainDlg->_calcTiltX() *  mDixTy; ;// mDixTx;
			}

			logStr.Format("TiltX : %.4f TiltY : %.4f", Task.SFR.dTilt_X, Task.SFR.dTilt_Y);
			putListLog(logStr);

			if (saveSfrLog(SFR_SECOND) == false)
			{
				iRtnFunction = -64000;
			}
			else if (fabs(Task.SFR.dTilt_X) > model.axis[theApp.MainDlg->TITLE_MOTOR_TX].m_dLimit_Err ||
				fabs(Task.SFR.dTilt_Y) > model.axis[theApp.MainDlg->TITLE_MOTOR_TY].m_dLimit_Err)
			{
				logStr.Format("Tilt º¸Á¤°ª Limit ÃÊ°ú : Tx : %.4f Ty : %.4f", Task.SFR.dTilt_X, Task.SFR.dTilt_Y);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -64000;
			}
			else
			{
				if (Task.bFirstAA == true)
				{
					//2Â÷
					logStr.Format("	AA Step 2Â÷[%d]", iRtnFunction);
					putListLog(logStr);
				}
				else
				{
					//1Â÷
					logStr.Format("	AA Step 1Â÷[%d]", iRtnFunction);
					putListLog(logStr);
				}
				iRtnFunction = 64200;		/* Focusing ¿Ï·á */
			}
		}
		else
		{
			iRtnFunction = 60100;		/* ´Ù½Ã ÃøÁ¤ */
		}

		if (Task.m_iCnt_Step_AA>model.strInfo_AF1.m_iStepCnt + 5)
		{
			logStr.Format("SFR ÃøÁ¤ ÃÖ´ë È½¼ö ÃÊ°ú.");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -64000;	/* Ä«¿îÆ® ÃÊ°ú½Ã ¿¡·¯ */
		}

		double z_move_dist = fabs(model.axis[theApp.MainDlg->TITLE_MOTOR_Z].pos[Bonding_Pos] - motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z));

		if (z_move_dist > 1.0)//0.5
		{
			logStr.Format("[SFR ÃøÁ¤] Z ¸ðÅÍ ÇÏ°­ °Å¸® ÃÊ°ú (1.0mm ÀÌ»ó ³»¸± ¼ö ¾ø½À´Ï´Ù.)");
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -64000;
		}
	}
	break;

	case 64200:		/* Trough Focus ÈÄ Center SFR °ª MAX À§Ä¡·Î Z¸ðÅÍ ÀÌµ¿ */
	{
		double dCurPos = 0.0;
		double dMovePos = Task.SFR.dMaxPos[0];//-3.970

											  //////////////////////////////////////////////////////////////////////////////////////////
											  //////////////////////////////////////////////////////////////////////////////////////////

		double z_move_dist = 0.0;
		//PCB Z­sÀÌ ¿ò©ÀÌ¹Ç·Î Tiltº¸Á¤½Ã PCB-Z­sÀÌ ÇÏ±ø Á…¾ßÇÔ.
		//
		//2Â÷ ³Ñ¾î°¡¸é¼­ ZÃà ¶ç¿ì´Â ºÎºÐ
		//Lens´Â ZÃà »ó½Â
		//Pcb´Â ZÃà ÇÏ°­
#if (____AA_WAY == PCB_TILT_AA)
		z_move_dist = fabs(model.axis[TITLE_MOTOR_Z].pos[Bonding_Pos] - dMovePos) * -1;	//1Â÷ ->2 Â÷ÀÌµ¿
#elif (____AA_WAY == LENS_TILT_AA)
		z_move_dist = fabs(model.axis[theApp.MainDlg->TITLE_MOTOR_Z].pos[Bonding_Pos] - dMovePos) * 1;	//1Â÷ ->2 Â÷ÀÌµ¿
#endif
		if (z_move_dist > 1.0)
		{
			logStr.Format("[SFR ÃøÁ¤] Z ¸ðÅÍ ÇÏ°­ °Å¸® ÃÊ°ú (1.0mm ÀÌ»ó ³»¸± ¼ö ¾ø½À´Ï´Ù.)");
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -64200;
			break;
		}

		motor.MoveAxis(theApp.MainDlg->TITLE_MOTOR_Z, ABS, dMovePos, /*model.strInfo_AF1.m_dMoveVel*/10.000, sysData.fMotorAccTime[theApp.MainDlg->TITLE_MOTOR_Z]);

		double ep = myTimer(true);

		while (1)
		{
			if (myTimer(true) - ep < 5000)
			{
				if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z) )
				{
					dCurPos = motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z);

					if (fabs(dCurPos - dMovePos) <= 0.003)
					{
						iRtnFunction = 64400;
						Task.PCBTaskTime = myTimer(true);

						break;
					}
				}
			}
			else
			{
				putListLog("Auto Focusing ñé LENS Z Motor ÀÌµ¿ ½Ã°£ ÃÊ°ú.");
				errMsg2(Task.AutoFlag, sLangChange);
				iRtnFunction = -64200;
				break;
			}
		}
	}

	break;


	case 64400:

		iRtn = theApp.MainDlg->_procTiltX();

		if (iRtn>0)
		{
			iRtnFunction = 64600;
		}
		else {
			iRtnFunction = -64400;
		}


		break;

	case 64600:

		iRtn = theApp.MainDlg->_procTiltY();

		if (iRtn>0)
		{
			iRtnFunction = 64700;
		}
		else {
			iRtnFunction = -64600;
		}


		break;

	case 64700:			/* Tilt º¸Á¤ ÈÄ Delay */
		if (Task.bFirstAA == true)
		{
			if ((myTimer(true) - Task.PCBTaskTime) > model.strInfo_Cam[CCD].m_iDelayTime)
			{
				iRtnFunction = 65000;
			}
			else
			{
				iRtnFunction = 64700;
			}
		}
		else
		{
			iRtnFunction = 65000;
		}
		break;

	case 65000:			/* Tilt ÈÄ MTF ÃøÁ¤ */
	{
		//Task.bFirstAA=true;
		if (Task.bFirstAA == true)//! Active Alignment 2¹øÂ° ¼öÇà 
		{
			Sleep(200);
			iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);

			Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
			theApp.MainDlg->autodispDlg->DrawBarGraph();	//65000

			if (iRtn > 0)
			{
				if (!theApp.MainDlg->_calcImageAlignment())
				{//-- Align º¸Á¤ Limit(ÀÓÀÇÀÇ °ª »ç¿ëÇÔ...1.0)
					sLangChange.LoadStringA(IDS_STRING605);	//ImageCenter NG.  X : %.3lf Y : %.3lf
					logStr.Format(sLangChange, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);
					iRtnFunction = -65000;
					break;
				}
				else
				{
					if (Task.iMTFCnt < 6)
					{
						if (fabs(Task.m_dShift_IMG_X) > sysData.m_dOcSpec.x &&
							fabs(Task.m_dShift_IMG_Y) > sysData.m_dOcSpec.y)
						{
							logStr.Format("ImageCenter ¿µ»ó Shift.  X %.3lf, Y %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
							putListLog(logStr);
							Sleep(200);
							iRtnFunction = 65500;
							break;
						}
						else
						{
							logStr.Format("ImageCenter ¿µ»ó Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf", sysData.m_dOcSpec.x, sysData.m_dOcSpec.y, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
							putListLog(logStr);
						}
					}
				}

				bool bMtfCheck2 = true;
				MIU.func_Set_InspImageCopy(UV_BEFORE_CHART, MIU.m_pFrameRawBuffer);			//========  AA UV BEFORE===
				Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);				// [AA UV BEFORE] #1
				MandoInspLog.func_LogSave_UVBefore();	// UVÀü Log Data ÀúÀå

				theApp.MainDlg->autodispDlg->DrawBarGraph();	//65000
				if (!Task.sfrResult && bMtfCheck2)
				{ 
					saveInspImage(AA_NG_IMAGE_SAVE, Task.m_iCnt_Step_AA_Total);
					logStr.Format("UVÀü SFR °Ë»ç NG.  \nAA RETRY ÇÏ½Ã°Ú½À´Ï±î?");//logStr.Format("SFR ÃÖÁ¾ °Ë»ç NG.  \nAA RETRY ÇÏ½Ã°Ú½À´Ï±î?");
					if (askMsg(logStr))
					{
						iRtnFunction = 60000;
						break;
					}
				}

				//saveSfrSumLog();//ÃÖÁ¾ Data log ÀúÀå

				double dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ;
				dCurPosX = motor.GetEncoderPos(Motor_PCB_X);
				dCurPosY = motor.GetEncoderPos(Motor_PCB_Y);
				dCurPosTH = motor.GetCommandPos(Motor_PCB_TH);
				dCurPosTX = motor.GetCommandPos(Motor_PCB_Xt);
				dCurPosTY = motor.GetCommandPos(Motor_PCB_Yt);
				dCurPosZ = motor.GetCommandPos(Motor_PCB_Z);
				if (!g_ADOData.func_AATaskToAAPcbMoter(Task.ChipID, dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ))
				{
					logStr.Format("DataBase »ý¼º ½ÇÆÐ.[%d]\n MS Office¸¦ ´Ý¾ÆÁÖ¼¼¿ä.", iStep);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -69999;
					break;
				}
				iRtnFunction = 69999;
			}
			else
			{
				logStr.Format("SFR ÃÖÁ¾ °Ë»ç ½ÇÆÐ.");
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -65000;
			}
			break;
		}
		else
		{//! Active Alignment 1¹øÂ° ¼öÇà
			iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
			if (iRtn > 0)
			{
				if (!theApp.MainDlg->_calcImageAlignment())	//¿©±â´Ù //¼öµ¿AA
				{//-- Align º¸Á¤ Limit(ÀÓÀÇÀÇ °ª »ç¿ëÇÔ...1.0)
					sLangChange.LoadStringA(IDS_STRING848);	//NG.  X : %.3lf Y : %.3lf
					logStr.Format(sLangChange, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);

					iRtnFunction = -65000;

					break;
				}
				else
				{
					if (fabs(Task.m_dShift_IMG_X) <= sysData.m_dOcSpec.x &&
						fabs(Task.m_dShift_IMG_Y) <= sysData.m_dOcSpec.y)
					{//X/Y º¸Á¤°ªÀÌ Spec InÀÏ °æ¿ì ´ÙÀ½ Step
						sLangChange.LoadStringA(IDS_STRING1302);	//¿µ»ó Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf
						logStr.Format(_T("		 ") + sLangChange, sysData.m_dOcSpec.x, sysData.m_dOcSpec.y, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
						putListLog(logStr);
						iRtnFunction = 69999;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
						break;
					}
					else
					{
						sLangChange.LoadStringA(IDS_STRING1301);	//¿µ»ó Shift.  X %.3lf, Y %.3lf
						logStr.Format(_T("		 ") + sLangChange, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
						putListLog(logStr);
						iRtnFunction = 65500;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
					}
				}
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING1074);	//SFR ¿µ»ó È¹µæ ½ÇÆÐ.
				errMsg2(Task.AutoFlag, sLangChange);
				iRtnFunction = -65000;
			}
		}
	}

	break;

	case 65500:
		if (!theApp.MainDlg->_MotorMove_IMG_Align())
		{
			iRtnFunction = -65500;
		}
		else
		{
			if (Task.iMTFCnt >= 5)
			{
				if (Task.bFirstAA == true) {
					iRtnFunction = 65520;// 65000;
					Task.PCBTaskTime = myTimer(true);
				}
				else
				{
					iRtnFunction = 69999;
				}
			}
			else
			{
				Task.iMTFCnt++;
				iRtnFunction = 65520;// 65000;
				Task.PCBTaskTime = myTimer(true);
			}
		}
		break;

	case 65520:
		if ((myTimer(true) - Task.PCBTaskTime) > iDelayTime)
		{
			iRtnFunction = 65000;
		}

		break;

	case 69000:								// ÃÖÁ¾ SFR °Ë»ç °á°ú ÀúÀå..
	{									// SFR °Ë»ç NG ¹ß»ý ½Ã 1È¸ Àç°Ë»ç ÈÄ ¾çºÒ ÆÇÁ¤.
		if (myTimer(true) - Task.PCBTaskTime > model.strInfo_Cam[CCD].m_iDelayTime * 2 || (myTimer(true)<Task.PCBTaskTime))
		{
			iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);		/* MTF°ª È¹µæ */

			Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
			theApp.MainDlg->autodispDlg->DrawBarGraph();	//69000

			if (iRtn>0)
			{
				if (Task.m_b_AA_Retry_Flag == false)
				{
					Task.m_b_AA_Retry_Flag = true;		// TRUEÀÎ °æ¿ì ´ÙÀ½¹ø¿¡´Â °Ë»ç °á°ú NG ½Ã ¾Ë¶÷ ¹ß»ý..
					iRtnFunction = 60000;				// 60000¿¡¼­ 1, 2Â÷ AA °Ë»ç °á°ú ÃÊ±âÈ­..
				}
				else
				{
					saveInspImage(AA_NG_IMAGE_SAVE, Task.m_iCnt_Step_AA_Total);
					Dio.setAlarm(ALARM_ON);

					logStr.Format("SFR ÃÖÁ¾ °Ë»ç NG. ÀÚµ¿ ÁøÇà ÇÏ½Ã°Ú½À´Ï±î?");
					if (askMsg(sLangChange))
					{
						Dio.setAlarm(ALARM_OFF);
						logStr.Format("NG ¹èÃâ ÇÏ½Ã°Ú½À´Ï±î. (¿¹:NG, ¾Æ´Ï¿À:OK)");
						if (askMsg(logStr)) {
							Task.m_bOkFlag = 0;
						}
						else {
							Task.m_bOkFlag = 1;
						}
						iRtnFunction = 69999;
					}
					else
					{
						Dio.setAlarm(ALARM_OFF);

						//Task.PauseLensStep = abs(Task.LensTask);
						Task.PausePCBStep = abs(Task.PCBTask);
						Task.AutoFlag = 2;		// ÀÏ½ÃÁ¤Áö
						theApp.MainDlg->AutoRunView(Task.AutoFlag);
						iRtnFunction = -65000;
					}
				}
			}
			else
			{
				logStr.Format("SFR ÃÖÁ¾ °Ë»ç (2Â÷) ½ÇÆÐ.");
				errMsg2(Task.AutoFlag, sLangChange);
				iRtnFunction = -65000;
			}
		}
	}
	break;
	case 69999:
	{
		double dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ;
		dCurPosX = motor.GetEncoderPos(Motor_PCB_X);
		dCurPosY = motor.GetEncoderPos(Motor_PCB_Y);
		dCurPosTH = motor.GetCommandPos(Motor_PCB_TH);
		dCurPosTX = motor.GetCommandPos(Motor_PCB_Xt);
		dCurPosTY = motor.GetCommandPos(Motor_PCB_Yt);
		dCurPosZ = motor.GetCommandPos(Motor_PCB_Z);
		//
		theApp.MainDlg->putListLog("	");
		logStr.Format("database teaching data save [%s]", Task.ChipID);
		theApp.MainDlg->putListLog("dbData/teachingData");
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA x: %lf/%lf", dCurPosX, model.axis[Motor_PCB_X].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA y: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Y].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA th: %lf/%lf", dCurPosX, model.axis[Motor_PCB_TH].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA tx: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Xt].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA ty: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Yt].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA z: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Z].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		//
		if (!g_ADOData.func_AATaskToAAPcbMoter(Task.ChipID, dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ))
		{
			logStr.Format("DataBase »ý¼º ½ÇÆÐ.[%d]\n MS Office¸¦ ´Ý¾ÆÁÖ¼¼¿ä.", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -69999;
			break;
		}
		if (Task.bFirstAA == true)
		{
			Task.m_timeChecker.Measure_Time(7);	//AA ¿Ï·á ½Ã°£
			Task.m_dTime_FineAA = Task.m_timeChecker.m_adTime[5] - Task.m_timeChecker.m_adTime[4];
			theApp.MainDlg->dispGrid();

			iRtnFunction = 110000;
		}
		else
		{
			iRtnFunction = 80000;
		}
	}

	break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int CPcbProcess2::UV_process(int iStep)
{
	CString sLog = _T("");
	CString sImgPath = _T("");
	//	int iRtn;
	int reCnt = 0;
	int iRtnFunction = iStep;
	double curMotorDist[3];

	switch (iStep)
	{
	case 110000:
		if (sysData.m_testRunMode == true)
		{
			iRtnFunction = 120000;

			break;
		}
		iRtnFunction = 110500;
		break;

	case 110500:
		if (theApp.MainDlg->Bonding_Pos_Ckeck())
		{
			sLog.Format("AA after PCB Z : %f , set Offset Z : %f", motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z), sysData.m_dOffset_Prev_UV_Z);	//AAÈÄ Lens Z : %f , ¼³Á¤ Offset Z : %f
			putListLog(sLog);

			theApp.MainDlg->MoveOffset_Prev_UV();	/* PCB X, Y, Z, Tx, Ty, PCB Th UV Àü offset ÀÌµ¿ */

			Sleep(200);
			if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z))
			{
				iRtnFunction = 110600;
				Task.PCBTaskTime = myTimer(true);
			}
			else
			{
				iRtnFunction = -110500;
			}
		}
		break;

	case 110600:
		Sleep(200);
		saveSfrLog(SFR_OFFSET_DATA);
		theApp.MainDlg->_getMTF(SFR_AFTER_OFFSET_MOVE);
		iRtnFunction = 110650;
		Task.PCBTaskTime = myTimer(true);

		break;

	case 110650:					//UVÀü MTF °Ë»ç ÁøÇà

		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();
		Task.bInsCenter = false;

		iRtnFunction = 110700;


		break;


	case 110700:

		iRtnFunction = 111000;
		Task.PCBTaskTime = myTimer(true);
		break;

	case 111000:	// UV °æÈ­
	{
		sLangChange.LoadStringA(IDS_STRING1122);
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, sLangChange);

		Task.PCBTaskTime = myTimer(true);

		if (sysData.m_iUVPass == 1)
		{
			sLog.Format("====UV Pass [%d]====", iStep);
			putListLog(sLog);
			iRtnFunction = 113500;
			break;
		}
		/*UVCommand.UV_Shutter_Open();
		if (sysData.nUvIndex == 1)
		{
		Sleep(100);
		UVCommand2.UV_Shutter_Open();
		}*/
		UVControl.Altis_UC_Controller_OnOff(1, 1);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(2, 1);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(3, 1);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(4, 1);
		Sleep(10);

		sLog.Format("===> UV ON");
		putListLog(sLog);
		Sleep(model.UV_Time);

		/*	UVCommand.UV_Shutter_Close();
		if (sysData.nUvIndex == 1)
		{
		Sleep(100);
		UVCommand2.UV_Shutter_Close();
		}*/
		UVControl.Altis_UC_Controller_OnOff(1, 0);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(2, 0);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(3, 0);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(4, 0);
		Sleep(10);

		sLog.Format("===> UV OFF [%d]", iStep);

		putListLog(sLog);
		iRtnFunction = 113500;
	}
	break;

	case 113500:
		sLangChange.LoadStringA(IDS_STRING1124);
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, sLangChange);//"UV °æÈ­ ¿Ï·á"

		Task.m_timeChecker.Measure_Time(8);	//UV ¿Ï·á ½Ã°£
		Task.m_dTime_UV = Task.m_timeChecker.m_adTime[8] - Task.m_timeChecker.m_adTime[7];


		if (myTimer(true) - Task.PCBTaskTime> 100)  // Delay Ã¼Å©
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 115000;
		}
		break;

	case 115000:
		curMotorDist[0] = fabs(motor.GetEncoderPos(Motor_Lens_X) - model.axis[Motor_Lens_X].pos[Bonding_Pos]);
		curMotorDist[1] = fabs(motor.GetEncoderPos(Motor_Lens_Y) - model.axis[Motor_Lens_Y].pos[Bonding_Pos]);
		curMotorDist[2] = fabs(motor.GetCommandPos(Motor_Lens_Z) - model.axis[Motor_Lens_Z].pos[Bonding_Pos]);

		if ((curMotorDist[0]<model.axis[Motor_Lens_X].m_dLimit_Err && curMotorDist[1]<model.axis[Motor_Lens_Y].m_dLimit_Err && curMotorDist[2]<model.axis[Motor_Lens_Z].m_dLimit_Err))
		{
			Task.PcbOnStage = 200;			// °Ë»ç ¿Ï·á.

			iRtnFunction = 115300;


		}

		break;

	case 115300:	//jump Step	
		if (!Dio.LensMotorGrip(false, true))
		{

			sLog.Format("Lens Unit Gripper ÇØÁ¦ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -115300;
			break;
		}
		Task.m_bOKLensPass = -1;//¸ÞÀÎ ³Ñ±è ¿Ï·á Lamp Ãë¼Ò


		Task.m_iFineThCnt = 0;
		iRtnFunction = 115500;
		break;
	case 115500: //UV¿Ï·áÈÄ ÃÖÁ¾°Ë»ç
		Task.m_bOkFlag = 0;
		work.m_iCnt_Output++;
		work.Save();
		iRtnFunction = 120000;
		theApp.MainDlg->_getMTF(SFR_AFTER_UV);
		break;


	default:
		sLangChange.LoadStringA(IDS_STRING1120);	//UV Process Step ¹øÈ£°¡ ºñÁ¤»óÀÔ´Ï´Ù.
		sLog.Format(sLangChange + _T(" [%d]"), iStep);
		errMsg2(Task.AutoFlag, sLog);
		break;
	}
	sLog.Empty();
	sImgPath.Empty();
	return iRtnFunction;
}

int	CPcbProcess2::Complete_FinalInsp(int iStep)
{
	CString sLog = _T("");
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iRtn = 0;
	int iRtnFunction = iStep;
	bool bMtfCheck4 = true;
	bool beolRtn = false;
	switch (iStep)
	{
	case 120000:	//¿ÏÁ¦Ç°
		Task.m_iFineThCnt = 0;
		theApp.MainDlg->setCamDisplay(3, 1);
		Sleep(dFinalDelay);

		//g_clPriInsp.func_insp_Voltage();

		iRtnFunction = 121000;
		break;
	case 121000:
		//g_clPriInsp.func_Insp_CurrentMeasure();
		iRtnFunction = 122000;
		break;

	case 122000:	//¿ÏÁ¦Ç° °Ë»ç¸¸ ÇØ´ç (Final °Ë»çÀü X/Y/T º¸Á¤)
		Sleep(dFinalDelay);
		iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
		Sleep(300);
		if (iRtn > 0)
		{
			//if (Task.m_iFineThCnt == 0)
			//{
			//	theApp.MainDlg->g_CalcImageAlign(true);	//¿ÏÁ¦Ç° rotation °Ë»ç 240131
			//}


			if (!theApp.MainDlg->_calcImageAlignment())	//-- Align º¸Á¤ Limit(ÀÓÀÇÀÇ °ª »ç¿ëÇÔ...1.0)
			{
				sLog.Format("NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
				putListLog(sLog);
				iRtnFunction = -122000;
				break;
			}
			else
			{

				//Æ¿ÆÃ°ª ¾È¸ÂÀ» °æ¿ì °ªÀÌ µ¿ÀÏÇÏ°Ô ¹Ýº¹µÊ.. 3È¸ Á¤µµ¸¸ ÇÏ°í ¾È¸ÂÀ¸¸é Spec IN ½ÃÅ°±â.., 
				if (Task.m_iFineThCnt > 3)	Task.m_dShift_IMG_TH = 0.0;
				if (fabs(Task.m_dShift_IMG_TH) <= sysData.dSpec_OC_Theta)//X/Y º¸Á¤°ªÀÌ Spec InÀÏ °æ¿ì ´ÙÀ½ Step
				{
					sLog.Format("	¿µ»ó Shift. Spec In(%.03lf) In : T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					iRtnFunction = 122500;
				}
				else
				{
					sLog.Format("	¿µ»ó Shift(Spec : %.03lf).  T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					Task.m_iFineThCnt++;
					iRtnFunction = 122100;
				}
			}
		}
		else
		{
			errMsg2(Task.AutoFlag, "MTF ÃøÁ¤ Àü SFR ¿µ»ó È¹µæ ½ÇÆÐ..");
			iRtnFunction = -122000;
		}
		break;

	case 122100:
		if (!theApp.MainDlg->_MotorMove_IMG_AlignTheta())
		{
			iRtnFunction = -122100;
		}
		else
		{
			Sleep(100);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 122200;	//Retry
		}
		break;
	case 122200:
		if ((myTimer(true) - Task.PCBTaskTime) > 300)
		{
			iRtnFunction = 122000;
			sLog.Format("	AA Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		break;
	case 122500:	//Final Àç °Ë»ç - È®ÀÎ¿ë
		theApp.MainDlg->setCamDisplay(3, 1);
		Sleep(dFinalDelay);

		MIU.func_Set_InspImageCopy(EOL_CHART, MIU.m_pFrameRawBuffer);	//================  ¿ÏÁ¦Ç° ¸ðµå chart
		Sleep(100);
		iRtnFunction = 122520;
		break;
	case 122520:
		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();

		Task.bInsCenter = false;

		Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);//EOL È­»ó #1

		if (Task.sfrResult == true)
		{
			sLangChange.LoadStringA(IDS_STRING229);
			putListLog("[°Ë»ç] MTF °Ë»ç ¼º°ø.");// Optic_Axis,
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING228);
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
		}
		Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		theApp.MainDlg->autodispDlg->DrawBarGraph();	//122520	Complete_FinalInsp
		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD, true);

		//vision.FnShmEdgeFind(MIU.m_pFrameRawBuffer);
		//g_clPriInsp.func_Insp_Color_reproduction(MIU.vTempBuffer);		//240625 shm ColorReproduction

		//MandoInspLog.func_LogSave_UVAfter(0);	//eol YUV LOG
		//MandoInspLog.func_LogSave_Shm_vertex();


		iRtnFunction = 122550;   //122600; <- ÀÌ¹ÌÁö save ÇÑÀå¸¸ ÇÒ¶§
		break;

	case 122550:
		
		iRtnFunction = 122560;
		break;
	case 122560:
		iRtnFunction = 122565;
		break;
	case 122565:
		//Sleep(500);
		iRtnFunction = 122570;
		break;
	case 122570:
		iRtnFunction = 122575;
		break;
	case 122575:
		iRtnFunction = 122580;
		break;
	case 122580:
		//mtf ÃøÁ¤
		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();

		Task.bInsCenter = false;

		//Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);//EOL È­»ó #1


		//Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		//theApp.MainDlg->autodispDlg->DrawBarGraph();		//122580   Complete_FinalInsp
		//vision.FnShmEdgeFind(MIU.m_pFrameRawBuffer);
		/////MandoInspLog.func_LogSave_UVAfter(1);	//eol _RAW.csv  ÀÌ¹°°Ë»çÈÄ °°ÀÌ ³²±è

		//MandoInspLog.func_LogSave_Shm_vertex();


		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD, true);
		iRtnFunction = 122590;  // 122590; Chart YUV - RAW 6500 RAW			IMHS	JUMP
		break;
	case 122590:
		iRtnFunction = 122600;
		break;
	case 122600:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -122600;
			break;
		}
		iRtnFunction = 122650;
		break;
	case 122650:
		iRtnFunction = 122700;
		break;
	case 122700:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING747);	//LensºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -122700;
			break;
		}

		Task.LensTaskTime = myTimer(true);
		//Task.m_iRetry_Opt = 0;
		iRtnFunction = 123000;
		break;
	case 123000:
		if (sysData.m_iDefectInspPass == 1)	// ÀÌ¹°°Ë»ç À¯¹«
		{
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
			putListLog(sLog);
			iRtnFunction = 123400;
		}
		else
		{
			//iRtnFunction = 123100;
		}
		break;
	case 123100:
		if (!motor.Pcb_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCBºÎ Dark À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123100;
		}
		else
		{
			sLog.Format("PCBºÎ Dark À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 123200;
		}
		break;
	case 123200:
		if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCBºÎ Dark Z À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123200;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 123300;
		break;
	case 123300:
		Sleep(100);
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Sleep(100);
			MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);		//¿ÏÁ¦Ç° dark
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123400;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z DAR Á¤ÁöÈ®ÀÎ ½Ã°£ ÃÊ°ú[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123200;
		}
		break;
	case 123400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -123400;
			break;
		}
		iRtnFunction = 123500;
		break;
	case 123500:
		if (sysData.m_iStaintInspPass == 1 && sysData.m_iDefectInspPass == 1)
		{
			iRtnFunction = 127000;
			break;
		}
		if (sysData.m_iIrChartUse == 1)
		{
			IrvAlignLed.Irv_Light_SetValue(model.m_iLedValue[LEDDATA_6500K], IR_OC);
		}
		else
		{
			LightControlthird.ctrlLedVolume(LIGHT_OC, model.m_iLedValue[LEDDATA_6500K]);
		}

		iRtnFunction = 123600;
		break;
	case 123600:
		if (!motor.Pcb_Motor_Move(OC_6500K_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING1023);	//PCBºÎ ¸ðÅÍ Defect °Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -123600;
		}
		else
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123650;
		}
		break;

	case 123650:
		//°Ë»ç Ãà ´ë±âÀ§Ä¡ È®ÀÎ
#if (__MACHINE_MODEL == MACHINE______2ST)
		if (motor.IsStopAxis(Motor_Insp_X))
		{
			sLog.Format("Insp X Á¤Áö È®ÀÎ [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 123670;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 10)
		{
			sLog.Format(_T("Insp x axis Á¤Áö È®ÀÎ ½ÇÆÐ[%d]"), iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123650;
		}
#endif
		break;
	case 123670:
		if (motor.INSP_Motor_MoveX(OC_6500K_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123700;
		}
		else
		{
			iRtnFunction = -123650;
			break;
		}
		break;
	case 123700:
		if (motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Complete[%d]", iStep);
			putListLog(sLog);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123800;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123700;
		}
		break;
	case 123800:
		if ((myTimer(true) - Task.PCBTaskTime) > 500)
		{
			iRtnFunction = 123850;
		}
		break;
	case 123850:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Task.PCBTaskTime = myTimer(true);

			//6500k ÀÌ¹ÌÁö ÀúÀå
			Sleep(300);
			MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);			//oc
			//ccdDlg->m_pSFRDlg->Raw_ImageSave(MIU.m_pFrameRawBuffer, MID_6500K_RAW);
			//MID_6500K_RAW ÀÌ¹ÌÁö ÀúÀå

			iRtnFunction = 123900;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Check Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123700;
		}
		break;
	case 123900:
		Sleep(dFinalDelay);
		if (sysData.m_iDefectInspPass == 1)	// ÀÌ¹°°Ë»ç À¯¹«
		{
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
			putListLog(sLog);
		}
		else
		{
			if (g_clPriInsp.func_Insp_Defect(MIU.vDefectMidBuffer_6500K, MIU.vDefectLowBuffer, true) == true)	//EOL
			{
				putListLog("[°Ë»ç] Defect °Ë»ç ¼º°ø.");
			}
			else
			{
				sLog.Format("[°Ë»ç] Defect °Ë»ç  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}
		}
		if (sysData.m_iStaintInspPass == 1)	// ÀÌ¹°°Ë»ç À¯¹«
		{
			sLog.Format(_T("[PASS] Stain Insp"), iStep);
			putListLog(sLog);

		}
		else
		{
			if (LGIT_MODEL_INDEX == M2_FF_MODULE)
			{
				//ÀÌ¹°±¤¿ø¿¡¼­ oc ÃøÁ¤ ¿äÃ»240725
				g_clPriInsp.g_GetIllumination(MIU.vDefectMidBuffer_6500K);
				Sleep(100);
			}
			if (g_clPriInsp.func_Insp_Stain(MIU.vDefectMidBuffer_6500K) == true)
			{
				putListLog("[°Ë»ç] Stain °Ë»ç ¼º°ø.");
			}
			else
			{
				sLog.Format("[°Ë»ç] Stain °Ë»ç  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}

		
			/*if (g_clPriInsp.func_Insp_Stain(MIU.vDefectMidBuffer_6500K) == true)
			{
				putListLog("[°Ë»ç] Stain °Ë»ç ¼º°ø.");
			}
			else
			{
				sLog.Format("[°Ë»ç] Stain °Ë»ç  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}*/
			//g_clPriInsp.func_Insp_Shm_Illumination(MIU.vTempBuffer);
		}
		

		iRtnFunction = 127590;	/// 127150;// 127000;			IMHS	JUMP
		break;
	case 127000:
		iRtnFunction = 127050;
		break;
	case 127050:
		iRtnFunction = 127100;
		break;
	case 127100:
		//Sleep(500);
		iRtnFunction = 127120;
		break;
	case 127120:

		iRtnFunction = 127150;
		break;
	case 127150:

		iRtnFunction = 127200;
		break;
	case 127200:
		iRtnFunction = 127590;
		break;
	case 127590:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127600;
		}
		else
		{
			sLog.Format("PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127000;
		}
		break;

	case 127600:
		if (!motor.INSP_Motor_MoveX(Wait_Pos))//, 0.0, false))
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127600;
		}
		else
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			Task.m_iRetry_Opt = 0;
			iRtnFunction = 127700;
		}
		
		break;
	case 127700:
		MandoInspLog.func_LogSave_UVAfter(0);	//eol _RAW.csv
		MIU.Close();
		iRtnFunction = 127800;
		break;
	case 127800:
		
		iRtnFunction = 127900;
		break;
	case 127900:
		Task.m_iRetry_Opt = 0;

		//g_FinalInspLog();

		work.iInspCnt_Output++;
		if (MandoInspLog.bInspRes == false)	work.iInspCnt_NG_Output++;
		theApp.MainDlg->dispInspResGrid();
		work.Save();
		work.Load();

		Task.m_timeChecker.Measure_Time(13);	//Final °Ë»ç ¿Ï·á ½Ã°£
		Task.m_dTime_TotalAA = Task.m_timeChecker.m_adTime[13] - Task.m_timeChecker.m_adTime[12];
		theApp.MainDlg->dispGrid();

		iRtnFunction = 128000;
		break;
	case 128000:
		Sleep(100);
		vision.clearOverlay(CCD);
		if (MandoInspLog.bInspRes == true)
		{
			vision.textlist[CCD].addList(50, 100, "PASS", M_COLOR_GREEN, 65, 85, "Arial");
		}
		else
		{
			vision.textlist[CCD].addList(50, 100, "FAIL", M_COLOR_RED, 80, 100, "Arial");
			MandoInspLog.func_DrawNG_Overlay(CCD);	//È­¸é Overlay¿¡ NG List Draw
			work.m_iCnt_NG_Output++;
		}
		vision.drawOverlay(CCD, true);

		//ccd¿µ»ó ÀüÈ¯

		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->setCamDisplay(3, 1);
		theApp.MainDlg->changeMainBtnColor(MAIN_DLG);
		iRtnFunction = 128100;
		break;

	case 128100:
		if (!motor.Lens_Motor_MoveX(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING748);	//LensºÎ ¸ðÅÍ ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -128100;
		}
		else
		{
			iRtnFunction = 129200;
		}
		break;
	case 129200:
		if (!motor.Lens_Motor_MoveXY(2, Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING748);	//LensºÎ ¸ðÅÍ ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -129200;
		}
		else
		{
			iRtnFunction = 129220;
		}
		break;

	case 129220:
		if (!motor.INSP_Motor_MoveX(Wait_Pos))//, 0.0, false))
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129220;
		}
		else
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 129300;
		}
		break;

		break;
	case 129300:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING987);	//PCB ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á [%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = 129350;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING986);	//PCB ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129300;
		}

		break;
	case 129350:
		//Dio.PcbGrip(false, false);
		iRtnFunction = 129400;
		break;
	case 129400:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB TX,TY ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 129500;
		}
		else
		{
			sLog.Format("PCB TX,TY ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129400;
		}
		break;

	case 129500:
		Task.m_bOkDispense = -1;	//¸ÞÀÎ µµÆ÷ ¿Ï·á Lamp Ãë¼Ò
		putListLog("		");
		putListLog("		");
		sLog.Format("			END =======%s", Task.ChipID);
		putListLog(sLog);
		putListLog("		");
		putListLog("		");
		Task.m_bPBStart = 0;	//¹ÙÄÚµå Á¤º¸ ÃÊ±âÈ­

		/*if (Task.bBcrAutoCount)
		{
			theApp.MainDlg->ChipIDCountPlus();
		}*/

		memset(Task.ChipID, 0x00, 256);
		sprintf_s(Task.ChipID, "EMPTY");
		theApp.MainDlg->func_ChipID_Draw();

		Task.m_timeChecker.Measure_Time(14);	//¿ÏÁ¦Ç° ¹èÃâ ½Ã°£
		Task.m_dTime_Total = Task.m_timeChecker.m_adTime[14] - Task.m_timeChecker.m_adTime[1];

		theApp.MainDlg->dispGrid();
		theApp.MainDlg->EpoxyTimeCheck(theApp.MainDlg->bEpoxyTimeChk);
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		Task.PCBTask = 10000;
		iRtnFunction = 10000;
		break;
	default:
		sLog.Format("Mando Test Process Step Number Error. [%d]", iStep);
		errMsg2(Task.AutoFlag, sLog);
		iRtnFunction = -iStep;
		break;
	}
	return iRtnFunction;
}

int	CPcbProcess2::func_MandoFinalSFR(int iStep)
{
	CString sLog = _T("");

	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	double offsetX = 0.0;
	double offsetY = 0.0;
	double offsetTh = 0.0;
	int iRtn = 0;
	int iRtnFunction = iStep;
	bool bUseDispense = true;	//µµÆ÷ Pass ¿©ºÎ

	if ((sysData.m_iDispensePass == 1))
	{
		bUseDispense = false;//µµÆ÷ Pass Mode ÀÌ°Å³ª, µµÆ÷ ¿Ï·á ÀÏ °æ¿ì
	}
	else
	{
		bUseDispense = true;
	}

	switch (iStep)
	{
	case 120000:
		if (sysData.m_testRunMode == true)
		{
			iRtnFunction = 122550;///// 125000;

			break;
		}
		//g_clPriInsp.func_insp_Voltage();
		iRtnFunction = 121000;
		break;

	case 121000:
		//g_clPriInsp.func_Insp_CurrentMeasure();
		iRtnFunction = 122000;
		break;
	case 122000:	//¿ÏÁ¦Ç° °Ë»ç¸¸ ÇØ´ç (Final °Ë»çÀü X/Y/T º¸Á¤)
		Sleep(dFinalDelay);
		iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
		if (iRtn > 0)
		{
			if (!theApp.MainDlg->_calcImageAlignment())	//-- Align º¸Á¤ Limit(ÀÓÀÇÀÇ °ª »ç¿ëÇÔ...1.0)
			{
				sLog.Format("NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
				putListLog(sLog);
				iRtnFunction = -122000;
				break;
			}
			else
			{
				//Æ¿ÆÃ°ª ¾È¸ÂÀ» °æ¿ì °ªÀÌ µ¿ÀÏÇÏ°Ô ¹Ýº¹µÊ.. 3È¸ Á¤µµ¸¸ ÇÏ°í ¾È¸ÂÀ¸¸é Spec IN ½ÃÅ°±â..,
				if (Task.m_iFineThCnt > 5)	Task.m_dShift_IMG_TH = 0.0;
				if (fabs(Task.m_dShift_IMG_TH) <= sysData.dSpec_OC_Theta)//X/Y º¸Á¤°ªÀÌ Spec InÀÏ °æ¿ì ´ÙÀ½ Step
				{
					sLog.Format("	¿µ»ó Shift. Spec In(%.03lf) In : T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					iRtnFunction = 122500;
				}
				else
				{
					sLog.Format("	¿µ»ó Shift(Spec : %.03lf).  T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					if (bUseDispense == false) //AAB, µµÆ÷ ¹Ì»ç¿ëÀÏ °æ¿ì->¿µ»ó Shift¾ÈÇÏ°í ÁøÇà
					{
						iRtnFunction = 122500;
					}
					else//¿µ»ó Shift ÁøÇà
					{
						Task.m_iFineThCnt++;
						iRtnFunction = 122100;
					}
				}
			}
		}
		else
		{
			if (askMsg("ÁøÇà?") == IDOK)
			{
				iRtnFunction = 122500;
				break;
			}
			else
			{
				errMsg2(Task.AutoFlag, "MTF ÃøÁ¤ Àü SFR ¿µ»ó È¹µæ ½ÇÆÐ..");
				iRtnFunction = -122000;
			}
		}
		break;

	case 122100:
		if (!theApp.MainDlg->_MotorMove_IMG_AlignTheta())
		{
			iRtnFunction = -122100;
		}
		else
		{
			Sleep(100);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 122200;	//Retry
		}
		break;
	case 122200:
		if ((myTimer(true) - Task.PCBTaskTime) > 300)
		{
			iRtnFunction = 122000;
			sLog.Format("	AA Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		break;
	case 122500:	//Final Àç °Ë»ç - È®ÀÎ¿ë
		theApp.MainDlg->setCamDisplay(3, 1);

		Sleep(dFinalDelay);
		MIU.func_Set_InspImageCopy(UV_AFTER_CHART, MIU.m_pFrameRawBuffer);		//========AA UV AFTER===
		Sleep(300);
		iRtnFunction = 122520;
		break;
	case 122520:

		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();

		Task.bInsCenter = false;

		Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);			// [AA UV AFTER] #1

		if (Task.sfrResult == true)
		{
			putListLog("[°Ë»ç] MTF °Ë»ç ¼º°ø.");// Optic_Axis,
		}
		else
		{
			putListLog("[°Ë»ç] MTF °Ë»ç ½ÇÆÐ.");// Optic_Axis,
		}
		Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		theApp.MainDlg->autodispDlg->DrawBarGraph();
		iRtnFunction = 122550;

		break;
	case 122550:			//jump step ¼öÁ¤ nono
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -122550;
			break;
		}

		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING747);	//LensºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -122550;
		}



		iRtnFunction = 122700;
		break;
	case 122700:

		sLog.Format("Lens X ´ë±â À§Ä¡ ÀÌµ¿ ¸í·É[%d]", iStep);
		putListLog(sLog);

		motor.Lens_Motor_MoveX(Wait_Pos, 0.0, false);		//Lens x µ¿½Ã ÀÌµ¿ 240108

		Task.m_iRetry_Opt = 0;
		iRtnFunction = 122800;
		break;
	case 122800:
		if (sysData.m_iDefectInspPass == 1)	// ÀÌ¹°°Ë»ç À¯¹«
		{
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
			putListLog(sLog);
			iRtnFunction = 123000;
		}
		else
		{
			iRtnFunction = 122850;
		}
		break;
	case 122850:
		if (!motor.Pcb_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCBºÎ Dark À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122850;
		}
		else
		{
			sLog.Format("PCBºÎ Dark À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 122900;
		}

		break;
	case 122900:
		if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCBºÎ Dark Z À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122900;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 122950;
		break;
	case 122950:
		if ((myTimer(true) - Task.PCBTaskTime) > 500)
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 122960;
		}
		break;
	case 122960:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);			//aa Dark
			iRtnFunction = 122970;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z DAR Á¤ÁöÈ®ÀÎ ½Ã°£ ÃÊ°ú[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -122900;
		}

		break;
	case 122970:

		iRtnFunction = 123000;
		break;
	case 123000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -123000;
			break;
		}
		iRtnFunction = 123050;


		break;
	case 123050:

		if (sysData.m_iDefectInspPass == 1 && sysData.m_iStaintInspPass == 1)
		{
			iRtnFunction = 124500;
			break;
		}
		LightControlthird.ctrlLedVolume(LIGHT_OC, model.m_iLedValue[LEDDATA_6500K]);
		iRtnFunction = 123055;

		break;
	case 123055:
		if (!motor.Pcb_Motor_Move(OC_6500K_Pos, 0.0, 0.0, 0.0, false))
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);	//PCBºÎ ¸ðÅÍ Defect °Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -123055;
		}
		else
		{
			iRtnFunction = 123056;
		}
		break;
	case 123056:
		if (!motor.INSP_Motor_MoveX(OC_6500K_Pos))
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);	//PCBºÎ ¸ðÅÍ Defect °Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			putListLog(sLog);
			iRtnFunction = -123056;
		}
		else
		{
			iRtnFunction = 123058;
			Task.PCBTaskTime = myTimer(true);
		}

		break;
	case 123058:
		if (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y) && motor.IsStopAxis(Motor_PCB_TH) &&
			motor.checkPcbMotorPos(OC_6500K_Pos))
		{
			sLog.Format("PcbºÎ OC_6500K_Pos À§Ä¡ È®ÀÎ ¿Ï·á[%d]", iStep);	//PCBºÎ ¸ðÅÍ Defect °Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			putListLog(sLog);
			iRtnFunction = 123100;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 25)
		{
			sLog.Format(_T("Pvb x,y,th axis Á¤Áö È®ÀÎ ½ÇÆÐ[%d]"), iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123058;
		}
		break;
	case 123100:
		if (motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Complete[%d]", iStep);
			putListLog(sLog);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123400;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123100;
		}
		break;
	case 123400:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Sleep(100);
			MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);		//aa oc
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 124000;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z 6500K Á¤ÁöÈ®ÀÎ ½Ã°£ ÃÊ°ú[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123100;
		}
		break;
	case 124000:
		if (sysData.m_iDefectInspPass == 1)	// ÀÌ¹°°Ë»ç À¯¹«
		{
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
			putListLog(sLog);
		}
		else
		{
			if (g_clPriInsp.func_Insp_Defect(MIU.vDefectMidBuffer_6500K, MIU.vDefectLowBuffer, true) == true)	//AA
			{
				putListLog("[°Ë»ç] Defect °Ë»ç ¼º°ø.");
			}
			else
			{
				sLog.Format("[°Ë»ç] Defect °Ë»ç  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}
		}
		if (sysData.m_iStaintInspPass == 1)	// ÀÌ¹°°Ë»ç À¯¹«
		{
			sLog.Format(_T("[PASS] Stain Insp"), iStep);
			putListLog(sLog);

		}
		else
		{

			if (LGIT_MODEL_INDEX == M2_FF_MODULE)
			{
				//ÀÌ¹°±¤¿ø¿¡¼­ oc ÃøÁ¤ ¿äÃ»240725
				g_clPriInsp.g_GetIllumination(MIU.vDefectMidBuffer_6500K);
				Sleep(100);
			}


			if (g_clPriInsp.func_Insp_Stain(MIU.vDefectMidBuffer_6500K) == true)
			{
				putListLog("[°Ë»ç] Stain °Ë»ç ¼º°ø.");
			}
			else
			{
				sLog.Format("[°Ë»ç] Stain °Ë»ç  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}
		}

		iRtnFunction = 124500;
		break;
	case 124500:

		MIU.Close();

		iRtnFunction = 125000;
		break;
	case 125000: 	//jump step º¯°æ nono
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 125500;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			sLog.Format("PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -125000;
		}
		break;
	case 125500:
		if (motor.IsStopAxis(Motor_Lens_X) && motor.checkLensMotorPos(Wait_Pos, 1))
		{
			sLog.Format("Lens XÃà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 126000;
			break;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 20)
		{
			sLog.Format("Lens X Á¤ÁöÈ®ÀÎ ½Ã°£ ÃÊ°ú[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -125500;
		}
		break;
	case 126000:
		if (!motor.Lens_Motor_MoveX(Wait_Pos))
		{
			sLog.Format("LENS X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);	//PCBºÎ ¸ðÅÍ Defect °Ë»ç À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -126000;
		}
		else
		{
			iRtnFunction = 127000;
		}
		break;
	case 127000:
		if (!motor.INSP_Motor_MoveX(Wait_Pos, 0.0, false))
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127000;
		}
		else
		{
			sLog.Format("INSP X Ãà ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127100;
		}
		break;
	case 127100:
		Task.m_iRetry_Opt = 0;
		MandoInspLog.func_LogSave_UVAfter(0);	//¸¸µµ Â÷·®¿ëCamera °Ë»ç Log ÀúÀå(°Ë»ç/º¸Á¤·® ÀúÀå -> ÃÖÁ¾Log)
		/*inspMES();
		MESCommunication.Messave();*/
		//! °Ë»ç Count Ã³¸®
		/*work.iInspCnt_Output++;
		if(MandoInspLog.bInspRes == false)	work.iInspCnt_NG_Output++;
		dispInspResGrid();
		work.Save();
		work.Load();*/

		Task.m_timeChecker.Measure_Time(13);	//Final °Ë»ç ¿Ï·á ½Ã°£
		Task.m_dTime_TotalAA = Task.m_timeChecker.m_adTime[13] - Task.m_timeChecker.m_adTime[12];
		///dispGrid();

		iRtnFunction = 127300;
		break;
	case 127300:
		Sleep(100);
		vision.clearOverlay(CCD);
		if (MandoInspLog.bInspRes == true)
		{
			vision.textlist[CCD].addList(50, 100, "PASS", M_COLOR_GREEN, 65, 85, "Arial");
		}
		else
		{
			vision.textlist[CCD].addList(50, 100, "FAIL", M_COLOR_RED, 80, 100, "Arial");
			MandoInspLog.func_DrawNG_Overlay(CCD);	//È­¸é Overlay¿¡ NG List Draw
			work.m_iCnt_NG_Output++;
		}
		vision.drawOverlay(CCD, true);

		//ccd¿µ»ó ÀüÈ¯

		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->setCamDisplay(3, 1);
		theApp.MainDlg->changeMainBtnColor(MAIN_DLG);
		iRtnFunction = 127400;
		break;

	case 127400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB ZÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127400;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 127500;
		break;
	case 127500:
		//°Ë»ç Ãà ´ë±âÀ§Ä¡ È®ÀÎ
#if (__MACHINE_MODEL == MACHINE______2ST)
		if (motor.IsStopAxis(Motor_Insp_X) && motor.checkInspMotorPos(Wait_Pos))
		{
			sLog.Format("Insp X ´ë±âÀ§Ä¡ ÀÌµ¿ ¸í·É[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127510;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 10)
		{
			sLog.Format(_T("Insp x axis Á¤Áö È®ÀÎ ½ÇÆÐ[%d]"), iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127500;
		}
#endif
		break;
	case 127510:
		if (motor.Lens_Motor_MoveY(Wait_Pos, 0.0, false))
		{
			sLog.Format("Lens Y ´ë±âÀ§Ä¡ ÀÌµ¿ ¸í·É[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127520;		//·»ÁîÂÊ Ä«¸Þ¶ó ¾øÀ½
		}
		else
		{
			sLog.Format("Lens Y ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -127510;
		}
		break;
	case 127520:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING987);	//PCB ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á [%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = 127900;

		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING986);	//PCB ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127500;
		}
		break;
	case 127900:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB TX,TY ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127950;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			sLog.Format("PCB TX,TY ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127900;
		}
		break;
	case 127950:

		if (motor.IsStopAxis(Motor_Lens_Y) && motor.checkLensMotorPos(Wait_Pos, 0))
		{
			iRtnFunction = 128000;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 1000 * 20)
		{
			sLog.Format(_T("Lens y axis ´ë±â À§Ä¡ È®ÀÎ ½ÇÆÐ[%d]"), iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127900;
		}

		break;
	case 128000:
		if (!motor.Lens_Motor_MoveXY(2, Wait_Pos, 0.0, 0.0, false))
		{
			sLog.Format("LensºÎ ¸ðÅÍ ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -128000;
		}
		else
		{
			iRtnFunction = 128200;
		}
		break;
	case 128200:
		iRtnFunction = 128500;

		break;
	case 128500:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 129000;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING745);	//LENSºÎ Tx, TyÃà ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ
			sLog.Format(sLangChange);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -128500;
		}
		break;
	case 129000:
		iRtnFunction = 129500;
		break;
	case 129500:
		Task.m_bOkDispense = -1;	//¸ÞÀÎ µµÆ÷ ¿Ï·á Lamp Ãë¼Ò
		putListLog("		");
		putListLog("		");
		sLog.Format("			END =======%s", Task.ChipID);
		putListLog(sLog);
		putListLog("		");
		putListLog("		");
		putListLog("		");
		putListLog("		");
		Task.m_bPBStart = 0;	//¹ÙÄÚµå Á¤º¸ ÃÊ±âÈ­
		/*if (Task.bBcrAutoCount)
		{
			theApp.MainDlg->ChipIDCountPlus();
		}
*/
		memset(Task.ChipID, 0x00, 256);
		sprintf_s(Task.ChipID, "EMPTY");
		theApp.MainDlg->func_ChipID_Draw();

		Task.m_timeChecker.Measure_Time(14);	//¿ÏÁ¦Ç° ¹èÃâ ½Ã°£
		Task.m_dTime_Total = Task.m_timeChecker.m_adTime[14] - Task.m_timeChecker.m_adTime[1];

		theApp.MainDlg->dispGrid();
		theApp.MainDlg->EpoxyTimeCheck(theApp.MainDlg->bEpoxyTimeChk);

		//Dio.PCBvaccumOn(VACCUM_OFF, false);


		//////////////////////////////////////////////////////////////////////////////////////////////////////
		Task.PCBTask = 10000;
		iRtnFunction = 10000;
		break;

	default:
		sLog.Format("Mando Test Process Step Number Error. [%d]", iStep);
		errMsg2(Task.AutoFlag, sLog);
		iRtnFunction = -iStep;
		break;
	}
	sLog.Empty();
	return iRtnFunction;
}


int CPcbProcess2::RunProc_LensNewPassPickup(int iUseStep)
{//! Lens Pickup¿¡ Á¦Ç° ³Ñ±è Step	(15000 ~ 16600)
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	switch (iUseStep)
	{
	case 15000:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens_ZÃà ´ëÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ");
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15000;
			break;
		}

		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15000;
			break;
		}

		if (Task.m_bOKLensPass != 1)//! Lens ³Ñ±è ¹Ì¿Ï·á ÀÏ °æ¿ì	 
		{
			if (!motor.Pcb_Motor_Move(Loading_Pos, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], 0))
			{
				logStr.Format("PCB %s ÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -15000;
				break;
			}
			iRtnFunction = 15100;
		}
		else
		{
			iRtnFunction = 15500;
		}
		break;
	case 15100:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ÇØÁ¦ ½ÇÆÐ[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -15100;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 15150;
		break;

	case 15150:
		if (Dio.LensMotorGripCheck(false, false))
		{
			iRtnFunction = 15200;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			iRtnFunction = -15150;
		}
		break;

	case 15200: // °ø±Þ
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,TyÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15200;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(1, Loading_Pos))
		{
			logStr.Format("Lens %s X,Y ÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15200;
			break;
		}
		logStr.Format("Lens °ø±Þ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 15250;
		break;

	case 15250:
		if (!motor.PCB_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("PCB loading Z ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15250;
		}
		else
		{
			iRtnFunction = 15252;
		}
		break;
	case 15252:
		if (motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			iRtnFunction = 15255;
		}
		else
		{
			logStr.Format("Lens Z À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15252;
		}
		break;

	case 15255:
		iRtnFunction = 15300;
		break;
	case 15300: // ¸ðÅÍ ±×¸³
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 15350;
			Sleep(500);
			Dio.LensMotorGrip(false, false);
			Sleep(500);
			Dio.LensMotorGrip(true, false);
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens ±×¸³ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15300;
		}
		break;

	case 15350://¸ðÅÍ ±×¸³ È®ÀÎ
		if (Dio.LensMotorGripCheck(true, false))
		{
			logStr.Format("Lens ±×¸³ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 15400;

			Sleep(800);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ÀüÁø ¼¾¼­ ½Ã°£ ÃÊ°ú..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15350;
		}
		break;
	case 15400:
		iRtnFunction = 15450;
		//Sleep(400);	//½Ç¸°´õ »ç¿ëÀ¸·Î PCB Stage UnGrip½Ã ¼¾¼­ °¨Áö ÈÄ, Delay
		/*if(Dio.HolderGrip(false,false))
		{
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 15450;
		}
		else
		{
		logStr.Format("Lens Gripper ÇØÁ¦ ½ÇÆÐ..[%d]", iUseStep);
		errMsg2(Task.AutoFlag,logStr);

		iRtnFunction = -15400;
		}*/
		break;

	case 15450:
		iRtnFunction = 15452;
		/*if(Dio.HolderGripCheck(false, false) )
		{
		iRtnFunction = 15452;
		}
		else if((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
		logStr.Format("PCB Stage Lens Gripper ÇØÁ¦ ½ÇÆÐ..[%d]", iUseStep);
		errMsg2(Task.AutoFlag,logStr);

		iRtnFunction = -15450;
		}*/
		//Sleep(600);
		break;
	case 15452:

		iRtnFunction = 15455;
		break;

	case 15455:

		iRtnFunction = 15500;
		break;
	case 15500: // ´ë±â À§Ä¡ ÀÌµ¿		//·»Áî ±×¸³»óÅÂ¿¡¼­ ½ÃÀÛÇÏ¸é ÀÌ¸® ¿Â´Ù ¼öÁ¤ÇÏÁö¸¶

		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15500;
		}
		//
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB waiting Z ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15500;
		}
		iRtnFunction = 15550;
		break;
	case 15550:
		if (motor.Pcb_Motor_Move(Wait_Pos))			//PCBºÎ ¸ðÅÍ ´ë±âÀ§Ä¡ ÀÌµ¿
		{
			iRtnFunction = 15800;
			logStr.Format("Pcb x,y ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á [%d]", iUseStep);
			putListLog(logStr);
		}
		else
		{
			logStr.Format("Pcb x,y ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -15550;
		}
		break;
	case 15800:

		Task.LensTask = 30000;	//pcb AAÀÏ¶§ lens ´Â Align ÀÌµ¿ÈÄ laser
		iRtnFunction = 15900;
		logStr.Format("PCB Epoxy Waiting[%d]", iUseStep);
		putListLog(logStr);
		break;
	case 15900:
		//pcb ´Â 
		//lens°¡ Align , laser¸¶Ä¡°í 
		//º»µùÀ§Ä¡·Î ºüÁö±â Àü±îÁö ´ë±â À§Ä¡·Î°¡¼­ ´ë±âÇØ¾ßµÈ´Ù.

		if (Task.interlockLens == 1)	//PCB AA »ç¿ë
		{
			Task.interlockLens = 0;	//PCB AA »ç¿ë
			iRtnFunction = 26000;
		}
		break;
	default:
		logStr.Format("Lens °ø±Þ µ¿ÀÛ Thread Step ¹øÈ£ ºñÁ¤»ó . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int	CPcbProcess2::RunProc_LensAlign(int iUseStep)			//(35000 ~ 39000)
{
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ¿µ»ó È¹µæ Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ÃøÁ¤Àü Delay
	switch (iUseStep)
	{
	case 35000:
		checkMessage();
		offsetX = offsetY = offsetTh = 0.0;
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("		PCB ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -35000;
			break;
		}
		iRtnFunction = 35450;	//2È£±â lens align pass 240723
		break;

#if 0
	case 35050:
		if (sysData.m_iAlignCamInspPass == 1)		//xxxxx
		{
			Task.m_iRetry_Opt = 0;
			iRtnFunction = 35400;
			break;
		}
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 35100;
		break;

	case 35100:
		Task.m_bOKLensPass = -1;//PCB¾ð·Îµå °úÁ¤¿¡¼­ ³Ñ±è»óÅÂ·Î ¸¸µé¾î³õÀ¸¹Ç·Î ÃÊ±âÈ­.
		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;


		if (Task.m_bOKLensPass != 1)//! Lens ³Ñ±è ¹Ì¿Ï·á ÀÏ °æ¿ì	 
		{
			iRtnFunction = 35150;
		}
		else
		{
			logStr.Format("Lens ³Ñ±è ¿Ï·á·Î Lens Align Pass![%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 35400;
		}
		break;
	case 35150:					//IM
		if (motor.INSP_Motor_MoveX(Lens_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1498);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Lens_Pos], iUseStep);
			iRtnFunction = 35151;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1499);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Lens_Pos], iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35150;
		}
		break;
	case 35151:
		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		//SetDigReference(LENS_Align_MARK);
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_LENS]);		// Align¸¸ Á¶¸í ON
		if (motor.Pcb_Motor_Move(Lens_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Lens_Pos))
			{
				logStr.Format("PCB Lens_Pos Align À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
				putListLog(logStr);
				Task.PCBTaskTime = myTimer(true);

				iRtnFunction = 35200;
			}
		}
		else
		{
			logStr.Format("PCB Lens Align À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35150;
		}
		break;
	case 35200:
		checkMessage();
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 35250;
		}
		break;

	case 35250:

		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB Lens Align Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]\n º¸Á¤ ¹ÌÀû¿ëÀ¸·Î ÁøÇàÇÏ½Ã°Ú½À´Ï±î?", Task.m_iRetry_Opt, iUseStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[LENS_Align_MARK] = 0;
				Task.d_Align_offset_y[LENS_Align_MARK] = 0;
				Task.d_Align_offset_th[LENS_Align_MARK] = 0;
				iRtnFunction = 35400;
			}
			else
			{
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 35250;
				logStr.Format("PCB Lens Align Àç°Ë»ç ½ÃÀÛ[%d]", iUseStep);
				putListLog(logStr);

				//iRtnFunction = -35250;
				//logStr.Format("PCB Align Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iUseStep);
				//errMsg2(Task.AutoFlag,logStr);
			}
			break;
		}

		offsetX = offsetY = offsetTh = 0.0;

		iRtn = theApp.MainDlg->procCamAlign(CAM1, LENS_Align_MARK, false, offsetX, offsetY, offsetTh);
		saveInspImage(LENS_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;
		offsetTh = 0;
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[LENS_Align_MARK] -= offsetX;
			Task.d_Align_offset_y[LENS_Align_MARK] -= offsetY;
			Task.d_Align_offset_th[LENS_Align_MARK] = 0.0;//offsetTh;

			Task.dPcbAlignTh = Task.d_Align_offset_th[LENS_Align_MARK];

			logStr.Format("PCB Lens Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM1, offsetX, offsetY, offsetTh);

			if (iRtnVal == 1)
			{
				iRtnFunction = 35300;				// º¸Á¤ ÀÌµ¿
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		PCB Lens Align [%d] ¿Ï·á [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);			//ÀÓ½Ã µî·Ï ¸¶Å© »èÁ¦

				logStr.Format("		PCB Lens Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;

				Task.m_timeChecker.Measure_Time(4);	//Laser º¯À§ ÃøÁ¤ ½ÃÀÛ ½Ã°£

				iRtnFunction = 35400;		// ¿Ï·á
			}
			else
			{
				logStr.Format("PCB Lens Align [%d] º¸Á¤ ¹üÀ§ ÃÊ°ú", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -35250;				// NG
			}
		}
		else
		{
			logStr.Format("PCB Lens Align [%d] ¸¶Å© ÀÎ½Ä ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);

			iRtnFunction = 35200;				//  Àç°Ë»ç 
			Sleep(300);
		}
		break;

	case 35300://- Sensor Align Retry
		if (motor.Pcb_Holder_Align_Move(Lens_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 35200;
		}
		else
		{
			logStr.Format("PCB Lens Align º¸Á¤ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35300;
		}
		break;
	case 35400:
		if (motor.INSP_Motor_MoveX(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1498);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Wait_Pos], iUseStep);
			iRtnFunction = 35450;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1499);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Wait_Pos], iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -35400;
			break;
		}
		break;
#endif
	case 35450:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			logStr.Format(sLangChange, iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -35450;
			break;
		}
		iRtnFunction = 35500;
		break;
	case 35500://! Lens-ZÃà ´ë±â À§Ä¡ ÀÌµ¿
		iRtnFunction = 39000;
		break;
	default:
		logStr.Format("Lens ¿ÜºÎ Align Thread Step ¹øÈ£ ºñÁ¤»ó [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int CPcbProcess2::RunProc_LensLoading(int iUseStep)
{//! Lens Pickup¿¡ Á¦Ç° ³Ñ±è Step	(15000 ~ 16600)
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	//Task.m_bOKLensPass = -1;//PCB¾ð·Îµå °úÁ¤¿¡¼­ ³Ñ±è»óÅÂ·Î ¸¸µé¾î³õÀ¸¹Ç·Î ÃÊ±âÈ­.

	switch (iUseStep)
	{
	case 40000:
		///Task.PCBTaskTime = myTimer(true);
		///iRtnFunction = 40920;	//Lens Thread¿¡¼­ ·»Áî ·Îµå ÁøÇà 240115

		break;
		break;
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens_ZÃà ´ëÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ");
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40000;
			break;
		}

		if (!motor.Lens_LaserAlign_Motor_MoveZ(Wait_Pos))
		{
			logStr.Format("LensLaserAlign ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40000;
			break;
		}

		if (Task.m_bOKLensPass != 1)//! Lens ³Ñ±è ¹Ì¿Ï·á ÀÏ °æ¿ì	 
		{

			Task.d_Align_offset_x[LENS_Align_MARK] = 0.0;
			Task.d_Align_offset_y[LENS_Align_MARK] = 0.0;
			if (!motor.Lens_LaserAlign_Motor_Move(Loading_Pos)) //, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], 0) )
			{
				logStr.Format("PCB %s ÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -40000;
				break;
			}
			//M_TRINITY ¸ðµ¨Àº ·¹ÀÌÀúÈÄ Æ¿Æ® ½ÉÇØ¼­ º»µùÀ§Ä¡°¥¶§ ´Ù½Ã ·¹ÀÌÀú Æ¿Æ®µÈ tx,ty °ªÀ¸·Î ÀÌµ¿ 
			/*	if (!motor.Pcb_Tilt_Motor_Move(Loading_Pos))
			{
			logStr.Format("PCB %s ÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40000;
			break;
			}*/
			iRtnFunction = 40100;
		}
		else
		{
			iRtnFunction = 40500;
		}
		break;
	case 40100:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ÇØÁ¦ ½ÇÆÐ[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -40100;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 40150;
		break;

	case 40150:
		if (Dio.LensMotorGripCheck(false, false))
		{
			iRtnFunction = 40170;
			Task.PCBTaskTime = myTimer(true);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			iRtnFunction = -40150;
		}
		break;
	case 40170:
		//logStr.Format("PCB %s ¼± ÀÌµ¿ ¸í·É ", MotorPosName[Bonding_Pos]);
		//putListLog(logStr);
		//if (!motor.Pcb_Motor_Move(Bonding_Pos,0.0, 0.0, 0.0, false))//if (!motor.Pcb_Motor_MoveY(Bonding_Pos))
		//{
		//	logStr.Format("PCB %s ¼± ÀÌµ¿ ¸í·É ½ÇÆÐ ", MotorPosName[Bonding_Pos]);
		//	delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
		//	iRtnFunction = -40170;
		//	break;
		//}
		//
		iRtnFunction = 40200;
		break;

	case 40200: // °ø±Þ 
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,TyÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40200;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(1, Loading_Pos))
		{
			logStr.Format("Lens %s X,Y ÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40200;
			break;
		}

		logStr.Format("Lens °ø±Þ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 40210;
		break;
	case 40210:
		iRtnFunction = 40250;
		break;
	case 40250:
		if (!motor.Lens_LaserAlign_Motor_MoveZ(Loading_Pos))
		{
			logStr.Format("LensLaserAlign loading Z ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40250;
			break;
		}
		iRtnFunction = 40251;
		break;
	case 40251:
		if (!motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens Z À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40251;
			break;
		}
		iRtnFunction = 40252;
		break;
	case 40252:
		Dio.LensMotorGrip(true, false);
		Sleep(500);
		iRtnFunction = 40255;
		break;

	case 40255:
		Dio.LensMotorGrip(false, false);
		Sleep(300);
		iRtnFunction = 40300;
		break;
	case 40300: // ¸ðÅÍ ±×¸³
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 40350;
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens ±×¸³ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40300;
		}
		break;

	case 40350://¸ðÅÍ ±×¸³ È®ÀÎ
		if (Dio.LensMotorGripCheck(true, false))
		{
			logStr.Format("Lens ±×¸³ ¿Ï·á[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 40400;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ÀüÁø ¼¾¼­ ½Ã°£ ÃÊ°ú..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40350;
		}
		break;

	case 40400:
		Sleep(500);	//½Ç¸°´õ »ç¿ëÀ¸·Î PCB Stage UnGrip½Ã ¼¾¼­ °¨Áö ÈÄ, Delay 0.2s
		iRtnFunction = 40455;
		break;

	case 40455:
		iRtnFunction = 40500;
		break;
	case 40500: // ´ë±â À§Ä¡ ÀÌµ¿
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40500;
			break;
		}
		iRtnFunction = 40550;
		break;
	case 40550:
		//
		if (!motor.Lens_LaserAlign_Motor_MoveZ(Wait_Pos))
		{
			logStr.Format("LensLaserAlign waiting Z ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40550;
			break;
		}
		iRtnFunction = 40900;
		break;
	case 40900:
		if (!motor.Lens_Motor_MoveX(Wait_Pos))
		{
			logStr.Format("Lens X ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40900;
			break;
		}
		iRtnFunction = 40920;
		break;

	case 40920:	//jump step
		iRtnFunction = 41000;
		break;
	default:
		logStr.Format("Lens °ø±Þ µ¿ÀÛ Thread Step ¹øÈ£ ºñÁ¤»ó . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess2::procProductComplete(int iStep)
{
	CString logStr = "";
	CString sSocketMsg = "";
	CString sBarCode = "";
	//
	int iRtnFunction = iStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ¿µ»ó È¹µæ Delay
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ÃøÁ¤Àü Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	double offsetX = 0.0;
	double offsetY = 0.0;
	double offsetTh = 0.0;
	int iTestPatCnt = 0;
	int iRtn = 0;
	//
	vision.clearOverlay(CCD);
	vision.drawOverlay(CCD);
	theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
	theApp.MainDlg->m_iCurCamNo = 0;
	theApp.MainDlg->changeMainBtnColor(MAIN_DLG);
	//
	switch (iStep)
	{
	case 11000:	// PCB MIU ¿¬°á	
				//MIU.INI_LOAD();
				//Sleep(1000);
		Task.oldLaserTx = 0.1;
		Task.oldLaserTy = 0.1;
		Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
		Task.PCBTaskTime = myTimer(true);
		//if (sysData.m_FreeRun == 0)
		//{
		//	bThread_MIUCheckRun = true;
		//	if (!MIUCheck_process() || gMIUDevice.CurrentState != 4)
		//	{
		//		sLangChange.LoadStringA(IDS_STRING452);	//CCD ¸ðµâ ¿µ»ó ÃÊ±âÈ­ ½ÇÆÐ.[%d]\n Á¦Ç° ¾ÈÂø »óÅÂ ¹× Á¦Ç° ºÒ·® È®ÀÎ ÇÏ¼¼¿ä.
		//		logStr.Format(sLangChange, iStep);
		//		errMsg2(Task.AutoFlag, logStr);
		//		iRtnFunction = -11000;
		//		break;
		//	}
		//	bThread_MIUCheckRun = false;
		//}
		iRtnFunction = 12000;
		break;
	case 12000:
		//È®ÀÎÇÊ¿ä norinda
		Task.m_bPBStart = 1;
		//if( Task.m_bPBStart ==  1 )
		//{
		//	func_Socket_Barcode();
		//}
		//else
		//{
		//	//! ¹ÙÄÚµå°¡ ¾ø±â ¶§¹®¿¡ Åë½Å »ç¿ë ¾ÈÇÔ.
		//	Task.iRecvLenACK[0] = Task.iRecvLenACK[1] = 0;
		//	Task.dTiltingManual[0] = Task.dTiltingManual[1] = 0.0;
		//	Task.dAlignManual[0] = Task.dAlignManual[0] = Task.dAlignManual[0] = 0.0;
		//}
		//iRtnFunction = 12900;			//laser º¯À§·Î Á¡ÇÁ
		//break;
		if (sysData.m_iAlignCamInspPass == 1)		//procProductComplete
		{
			logStr.Format("¿ÏÁ¦Ç° Align Pass[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12800;
			break;
		}
		if (!motor.Pcb_Tilt_Motor_Move(Com_Laser_Pos))
		{
			logStr.Format("PCB tx,ty Com_Laser_Pos ÀÌµ¿ ½ÇÆÐ");
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -12000;
			break;
		}
		iRtnFunction = 12900;			//laser º¯À§·Î Á¡ÇÁ
		break;
		
		
		break;
	case 12100:
		if (motor.INSP_Motor_MoveX(CompleteAlign_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("INSP X Ãà ¿ÏÁ¦Ç° Align À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12110;
		}
		else
		{
			logStr.Format("INSP X Ãà ¿ÏÁ¦Ç° Align À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12100;
		}
		break;
	case 12110:

		Task.m_iRetry_Opt = 0;
		//SetDigReference(PCB_Holder_MARK);
		LightControlthird.ctrlLedVolume(LIGHT_OC, model.m_iLedValue[LEDDATA_HOLDER]);		// Align¸¸ Á¶¸í ON

		Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_th[PCB_Chip_MARK] = 0;

		if (motor.Pcb_Motor_Move(CompleteAlign_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb ¿ÏÁ¦Ç° Align À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12200;
		}
		else
		{
			logStr.Format("Pcb ¿ÏÁ¦Ç° Align À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12100;
		}
		break;
	case 12200:
		if (motor.PCB_Z_Motor_Move(CompleteAlign_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb ¿ÏÁ¦Ç° Align ZÀ§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12400;
		}
		else
		{
			logStr.Format("Pcb ¿ÏÁ¦Ç° Align ZÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12200;
		}
		break;

	case 12400:
		checkMessage();
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 12500;
		}
		break;
	case 12500:

		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB ¿ÏÁ¦Ç° Align Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]\n º¸Á¤ ¹ÌÀû¿ëÀ¸·Î ÁøÇàÇÏ½Ã°Ú½À´Ï±î?", Task.m_iRetry_Opt, iStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
				iRtnFunction = 12800;
			}
			else
			{
				iRtnFunction = -12500;
				logStr.Format("PCB Align Àç°Ë»ç %d È¸ ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iStep);
				errMsg2(Task.AutoFlag, logStr);
			}
			break;

		}
		//-----------------------------------
		offsetX = offsetY = offsetTh = 0.0;
		iRtn = theApp.MainDlg->procCamComAlign(CAM1, PCB_Chip_MARK, false, offsetX, offsetY, offsetTh);

		offsetTh = 0.0;
		saveInspImage(PCB_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;
		if (iRtn == 1)
		{
			Task.d_Align_offset_x[PCB_Chip_MARK] += offsetX;
			Task.d_Align_offset_y[PCB_Chip_MARK] += offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] += offsetTh;

			logStr.Format("PCB ¿ÏÁ¦Ç° Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();
			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM1, offsetX, offsetY, offsetTh); // PCBÀÌÀç±â ¿òÁ÷ÀÌ´Â ºÎºÐÀÌ¶û Limit Check ÇÒ¶§ CAM1ÀÓ

			if (iRtnVal == 1)
			{
				iRtnFunction = 12700;				// º¸Á¤ ÀÌµ¿
			}
			else if (iRtnVal == 2)
			{
				iLaser_Pos = 0;
				logStr.Format("		PCB ¿ÏÁ¦Ç° Align [%d] ¿Ï·á [%d]", Task.m_iRetry_Opt, iStep);
				putListLog(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				logStr.Format("		PCB ¿ÏÁ¦Ç° Align Complete :  %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				Task.dSocketRecvTime = myTimer(true);
				iRtnFunction = 12800;

			}
			else
			{
				logStr.Format("PCB ¿ÏÁ¦Ç° Align [%d] º¸Á¤ ¹üÀ§ ÃÊ°ú[%d]", Task.m_iRetry_Opt, iStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -12500;
			}
		}
		else
		{
			logStr.Format("PCB ¿ÏÁ¦Ç° Align [%d] ¸¶Å© ÀÎ½Ä ½ÇÆÐ[%d]", Task.m_iRetry_Opt, iStep);
			putListLog(logStr);
			iRtnFunction = 12500;				//  Àç°Ë»ç 
		}
		break;

	case 12700://!! º¸Á¤·® ÀÌµ¿.- Retry
		if (motor.Pcb_Holder_Align_Move(CompleteAlign_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 12200;
		}
		else
		{
			logStr.Format("Pcb ¿ÏÁ¦Ç° Align º¸Á¤ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12700;
		}
		break;
	case 12800:
		//LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		//·¹ÀÌÀú °Ë»çÀü off
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCBºÎ zÃà ¸ðÅÍ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]
			logStr.Format(sLangChange, iStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -12800;
			break;
		}

		iRtnFunction = 12850;


		break;
	case 12850:
		iLaser_Pos = 0;
		vision.clearOverlay(CAM1);
		//vision.drawOverlay(CAM1);
		//¿ÜºÎ º¯À§ÃøÁ¤°ª Read
#if (____AA_WAY == LENS_TILT_AA)//PCB_TILT_AA) 
		if (g_ADOData.func_Get_RecordData(Task.ChipID))//DB¿¡ Data°¡ ÀÖÀ» °æ¿ì
		{

			Task.dAAPcbMotor[0] = g_ADOData.adoRegist.dPCBMotor[0];   //PCB X
			Task.dAAPcbMotor[1] = g_ADOData.adoRegist.dPCBMotor[1];   //PCB Y
			Task.dAAPcbMotor[2] = g_ADOData.adoRegist.dPCBMotor[2];   //PCB TH
																	  //TX[3] , TY[4]
			Task.dAAPcbMotor[3] = g_ADOData.adoRegist.dPCBMotor[3];   //PCB TX 
			Task.dAAPcbMotor[4] = g_ADOData.adoRegist.dPCBMotor[4];   //PCB TY 
																	  //
			Task.dAAPcbMotor[5] = g_ADOData.adoRegist.dPCBMotor[5];   //PCB Z
			Task.dAlignManual[0] = g_ADOData.adoRegist.dOffset_Align[0];  //AlignOffsetX
			Task.dAlignManual[1] = g_ADOData.adoRegist.dOffset_Align[1];  //AlignOffsetY
			Task.dAlignManual[2] = g_ADOData.adoRegist.dOffset_Align[2];  //AlignOffsetT
											
			logStr.Format("PCB È­»ó°Ë»ç Chart Pos X(%.3lf), Y(%.3lf)", Task.dAAPcbMotor[0], Task.dAAPcbMotor[1]);
			putListLog(logStr);
			//
			logStr.Format("PCB È­»ó°Ë»ç TX(%.3lf), TY(%.3lf)", Task.dAAPcbMotor[3], Task.dAAPcbMotor[4]);
			putListLog(logStr);

			if (fabs(Task.dAAPcbMotor[0] - model.axis[Motor_PCB_X].pos[Bonding_Pos]) > 5.0)
			{
				logStr.Format("Pcb È­»ó°Ë»ç X spec Over[%d]", iStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -12850;
				break;
			}
			if (fabs(Task.dAAPcbMotor[1] - model.axis[Motor_PCB_Y].pos[Bonding_Pos]) > 5.0)
			{
				logStr.Format("Pcb È­»ó°Ë»ç Y spec Over[%d]", iStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -12850;
				break;
			}
			iRtnFunction = 12900;
			break;
		}
		else
		{
			Task.dAAPcbMotor[0] = model.axis[Motor_PCB_X].pos[Bonding_Pos];   //PCB X
			Task.dAAPcbMotor[1] = model.axis[Motor_PCB_Y].pos[Bonding_Pos];   //PCB Y
			Task.dAAPcbMotor[2] = model.axis[Motor_PCB_TH].pos[Bonding_Pos];   //PCB TH
																			   //TX[3] , TY[4]
			Task.dAAPcbMotor[3] = model.axis[Motor_PCB_Xt].pos[Bonding_Pos];   //PCB TX 	model.m_InspPcbTilt[0];//
			Task.dAAPcbMotor[4] = model.axis[Motor_PCB_Yt].pos[Bonding_Pos];   //PCB TY 	model.m_InspPcbTilt[1];//

																			   //
			Task.dAAPcbMotor[5] = model.axis[Motor_PCB_Z].pos[Bonding_Pos];   //PCB Z

			iRtnFunction = 12900;

			//iRtnFunction = 12900;
			//logStr.Format("ÀÏÄ¡ ¸ðµ¨ ¾ø½À´Ï´Ù.\ bonding À§Ä¡·Î ÁøÇà ÇÏ½Ã°Ú½À´Ï±î?");	//ÀüÃ¼ ¿øÁ¡ º¹±Í¸¦ ½ÇÇà ÇÏ½Ã°Ú½À´Ï±î?
			//if (askMsg(logStr))
			//{

			//	
			//	break;
			//}
			/*logStr.Format("ÀÏÄ¡ ¸ðµ¨ ¾ø½À´Ï´Ù.[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12850;*/
			//ÀÏÄ¡ ¸ðµ¨ ¾øÀ» °æ¿ì ¼öµ¿ Æ¼Äª°ª Ã¢À¸·Î.
			/*Task.m_iRtnTiltingDlg = 2;
			TiltingManualdlg->ShowWindow(SW_SHOW);
			iRtnFunction = 12870;*/
		}


#else //#elif (____AA_WAY == LENS_TILT_AA)
		Task.dTiltingManual[0] = 0.0;
		Task.dTiltingManual[1] = 0.0;
		iRtnFunction = 12900;
		/*
		Task.dTiltingManual[0] = g_ADOData.adoRegist.dOffset_TiltTx;
		Task.dTiltingManual[1] = g_ADOData.adoRegist.dOffset_TiltTy;

		Task.dAlignManual[0] = g_ADOData.adoRegist.dOffset_Align[0];
		Task.dAlignManual[1] = g_ADOData.adoRegist.dOffset_Align[1];
		Task.dAlignManual[2] = g_ADOData.adoRegist.dOffset_Align[2];
		*/

		break;
#endif
		break;
	case 12870:
		if (Task.m_iRtnTiltingDlg == 0)
		{
			logStr.Format("º¯À§ÃøÁ¤ Data ÀÔ·Â Áß ÀÚµ¿¸ðµå Á¤Áö ¼±ÅÃ[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12870;
			break;
		}
		else if (Task.m_iRtnTiltingDlg == 1)
		{
			logStr.Format("¿ÜºÎ º¯À§ Tilting ¿Ï·á À§Ä¡ : Tx=%.03lf, Ty=%.03lf", Task.dTiltingManual[0], Task.dTiltingManual[1]);
			putListLog(logStr);

			Task.m_timeChecker.Measure_Time(4);	//Laser º¯À§ ÃøÁ¤ ½ÃÀÛ ½Ã°£
												//iRtnFunction = 12900;//pcb aa¶Ç ·¹ÀÌÀú ÇÏµµ·Ï 200708

			iRtnFunction = 12900;

		}
		break;
	case 12900:
		if (sysData.m_iLaserInspPass == 1)		//¿ÏÁ¦Ç°
		{
			logStr.Format("¿ÏÁ¦Ç° Laser pass");
			putListLog(logStr);
			iRtnFunction = 13300;		//¿ÏÁ¦Ç°½Ã ·¹ÀÌÀú ÆÐ½º
			break;
		}
		if (motor.INSP_Motor_MoveX(Com_Laser_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Laser º¯À§ ÃøÁ¤ ¿ÜºÎ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12901;


		}
		else
		{
			logStr.Format("INSP X Ãà Com_Laser_Pos ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12900;
			break;
		}
		break;
	case 12901:
		


		if (motor.Pcb_Motor_Move_Laser(iLaser_Pos))	//if(motor.Pcb_Motor_Move_Laser(iLaser_Pos, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]))
		{
			if (motor.PCB_Z_Motor_Move(Com_Laser_Pos))//Laser_Lens_Pos))
			{
				Task.PCBTaskTime = myTimer(true);
				logStr.Format("Laser º¯À§ ÃøÁ¤ ¿ÜºÎ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
				putListLog(logStr);
				iRtnFunction = 12910;// 12950;
			}
			else
			{
				logStr.Format("PCB Com_Laser_Pos Z ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -12900;
				break;
			}
		}
		else
		{
			logStr.Format("Laser º¯À§ ÃøÁ¤ ¿ÜºÎ À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12900;
		}

		break;
	case 12910:
		if (iLaser_Pos > 0)
		{
			iRtnFunction = 12950;
			break;
		}
		iRtnFunction = 12920;
		break;

	case 12920:
		iRtnFunction = 12950;
		break;
		//laser½Ç¸°´õ ÇÏ°­ END
	case 12950:
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 13100;
		}
		break;
	case 13100:
		Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Sleep(100);
		logStr.Format("º¯À§ ¼¾¼­ %lf[%d] - ¿ÜºÎ - %d À§Ä¡", Task.m_Laser_Point[iLaser_Pos], iStep, iLaser_Pos + 1);
		putListLog(logStr);
		LaserPos[iLaser_Pos].x = motor.GetEncoderPos(Motor_PCB_X);
		LaserPos[iLaser_Pos].y = motor.GetEncoderPos(Motor_PCB_Y);
		LaserValue[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];

		if (Task.m_Laser_Point[iLaser_Pos] != 0)
		{
			if (iLaser_Pos == 3)
			{
				if (theApp.MainDlg->func_Check_LaserValueErr(LaserValue) == false)
				{
					logStr.Format("Laser ÃøÁ¤°ª ÀÌ»ó..¿ÜºÎ º¯À§ ÃøÁ¤°ªÀÌ ÀÌ»ó..[%d]\n ÃøÁ¤°ª:%.04lf, %.04lf, %.04lf, %.04lf ", iStep, LaserValue[0], LaserValue[1], LaserValue[2], LaserValue[3]);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -13100;
					break;
				}

				if (theApp.MainDlg->_calcLaserTilt(LaserPos, LaserValue, Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]))
				{
					/*	logStr.Format("		º¸Á¤·® Tx: %.04lf, Ty: %.04lf", Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0] );
					putListLog(logStr);
					logStr.Format("		³»ºÎ º¸Á¤·® ÆíÂ÷ Tx: %.04lf, Ty: %.04lf", Task.dTiltingManual[0], Task.dTiltingManual[1] );
					putListLog(logStr);
					int Rnd = TiltAlignLimitCheck(Task.d_Align_offset_xt[0] + Task.dTiltingManual[0], Task.d_Align_offset_yt[0] + Task.dTiltingManual[1] );
					*/


					//Task.m_dataOffset_x[0] =  Task.d_Align_offset_xt[PCB_Holder_MARK];
					//Task.m_dataOffset_y[0] =  Task.d_Align_offset_yt[PCB_Holder_MARK];
					logStr.Format("		º¸Á¤·® Tx: %.04lf, Ty: %.04lf", Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]);
					putListLog(logStr);
					logStr.Format("		¿ÜºÎ º¸Á¤·® ÆíÂ÷ Tx: %.04lf, Ty: %.04lf", Task.dTiltingManual[0], Task.dTiltingManual[1]);
					putListLog(logStr);
					//int Rnd = TiltAlignLimitCheck(Task.d_Align_offset_xt[PCB_Holder_MARK],Task.d_Align_offset_yt[PCB_Holder_MARK]);
					//int Rnd = TiltAlignLimitCheck(Task.d_Align_offset_xt[0] + Task.dTiltingManual[0], Task.d_Align_offset_yt[0] + Task.dTiltingManual[1] );
					int Rnd = theApp.MainDlg->TiltAlignLimitCheck(Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]);
					if (Rnd == 2)
					{
						Task.m_timeChecker.Measure_Time(5);	//Laser º¯À§ ÃøÁ¤ ¿Ï·á ½Ã°£
						Task.m_dTime_LaserDpm = Task.m_timeChecker.m_adTime[5] - Task.m_timeChecker.m_adTime[4];
						//dispGrid();
						iRtnFunction = 13300;

					}
					else if (Rnd == 1)
					{
						iRtnFunction = 13200;	//º¸Á¤·® ÀÌµ¿		
					}
					else
					{
						logStr.Format("Laser Tilt  º¸Á¤°ª Limit¸¦ ÃÊ°ú ÇÏ¿´½À´Ï´Ù.[%d]", iStep);
						errMsg2(Task.AutoFlag, logStr);
						iRtnFunction = -13100;
					}
				}
			}
			else
			{
				iRtnFunction = 12900;
				iLaser_Pos++;
			}
		}
		else
		{
			iRtnFunction = 13100;
		}
		break;
	case 13200:
		//if(motor.Pcb_Move_Tilt(Task.d_Align_offset_xt[0] + Task.dTiltingManual[0],Task.d_Align_offset_yt[0] + Task.dTiltingManual[1] ))
		if (motor.Pcb_Move_Tilt(Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]))
		{
			iLaser_Pos = 0;
			iRtnFunction = 12900;
		}
		break;

	case 13300:
		iRtnFunction = 13310;
		break;
	case 13310:
		iRtnFunction = 13350;
		break;
		//laser½Ç¸°´õ »ó½Â END
	case 13350:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 14000;
		}
		else
		{
			logStr.Format("PCB Z ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = -13350;
		}
		break;
	case 14000:

		iRtnFunction = 14100;
		break;
	case 14100:

		iRtnFunction = 14200;
		break;

	case 14200:
		checkMessage();
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 14300;
			Task.m_iRetry_Opt = 0;
		}
		break;
	case 14300:

		iRtnFunction = 14400;				// ¿Ï·á
		break;
	case 14400:
		iRtnFunction = 14500;
		break;
	case 14500:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z ´ë±â À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 15200;
		}
		else
		{
			logStr.Format("PCB Z ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = -14500;
		}
		break;
	case 15200:
		iRtnFunction = 19600;
		break;

	case 19600: // º»µù À§Ä¡ ÀÌµ¿ 
				//if(motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Holder_MARK] + Task.dAlignManual[0], Task.d_Align_offset_y[PCB_Holder_MARK] + Task.dAlignManual[1], Task.d_Align_offset_th[PCB_Holder_MARK] + Task.dAlignManual[2]) )
		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("PCB Bonding º¸Á¤ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 19700;
		}
		else
		{
			logStr.Format("PCB Bonding À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -19600;
		}
		break;
	case 19700:
		//if (!motor.INSP_Motor_MoveX(OC_6500K_Pos, 0.0, false))
		//{
		//	logStr.Format("INSP X Ãà ÀÌ¹° À§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iStep);
		//	errMsg2(Task.AutoFlag, logStr);
		//	iRtnFunction = -19700;
		//}
		//else
		//{
		//	logStr.Format("INSP X Ãà ÀÌ¹° À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iStep);
		//	putListLog(logStr);
		//	iRtnFunction = 19800;//pcb aaµµ ·¹ÀÌÀú ÇÏµµ·Ï 200708
		//}

#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 19800;
#elif (____AA_WAY == LENS_TILT_AA)
		if (motor.Pcb_Move_Tilt(Task.dAAPcbMotor[3], Task.dAAPcbMotor[4]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 19800;
		}
		else
		{
			logStr.Format(_T("PCB Tilt Bonding Pos Move Fail[%d]"), iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -19700;
		}
		
#endif
		break;
	case 19800: //zÃà º»µù À§Ä¡ ÀÌµ¿
		if (!motor.PCB_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format(_T("PCB Z Bonding Pos Move Fail[%d]"), iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -19800;
		}
		iRtnFunction = 19900;
		break;
	case 19900:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			
			if (sysData.m_iIrChartUse == 1)
			{
				LightControlSecond.ctrlLedVolume(0, model.m_iLedValue[LEDDATA_TOP2_CHART]);
			}
			else
			{
				LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP1_CHART]);
			}


			//if( func_MIU_ConnectLiveCheck() == false)		//MIU ¿µ»ó Check
			//{
			//	iRtnFunction = -19900;
			//	break;
			//}
			theApp.MainDlg->setCamDisplay(3, 1);

			iRtnFunction = 120000;//¿ÏÁ¦Ç° °Ë»ç
		}
		break;

	default:
		sLangChange.LoadStringA(IDS_STRING1004);	//PCB ¿ÏÁ¦Ç° °Ë»ç Thread Step ¹øÈ£ ºñÁ¤»ó . [%d]
		logStr.Format(sLangChange, iStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -20000;
		break;
	}
	return iRtnFunction;
}

int CPcbProcess2::RunProc_LENS_LensLoad(int iLensStep)
{
	int iRtnFunction = iLensStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;		//PCB ¿µ»ó È¹µæ Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ÃøÁ¤Àü Delay

	switch (iLensStep)
	{
	case 30000:
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 50000;
		break;
#endif
		Task.interlockLens = 0;			//Lens AA »ç¿ë
		if (Task.m_bOKLensPass == 1)//! Lens ³Ñ±è ¹Ì¿Ï·á ÀÏ °æ¿ì	 
		{
			logStr.Format("Lens ³Ñ±è ¿Ï·á »óÅÂ [%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 39500;
			break;
		}
		iRtnFunction = 30500;
		break;
	case 30500:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 30700;
		}
		else
		{
			logStr.Format("Lens_ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30500;
		}

		break;
	case 30700:
		if (!motor.Lens_LaserAlign_Motor_MoveZ(Wait_Pos))
		{
			logStr.Format("LensLaserAlign ZÃà ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iLensStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -30700;
			break;
		}
		iRtnFunction = 30800;
		break;
	case 30800:
		if (!motor.Lens_LaserAlign_Motor_Move(Loading_Pos))
		{
			logStr.Format("PCB %s ÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -30800;
			break;
		}
		iRtnFunction = 31000;
		break;
	case 31000:
		if (motor.Lens_Motor_MoveXY(2, Wait_Pos))
		{
			iRtnFunction = 31500;		//·»ÁîÂÊ Ä«¸Þ¶ó ¾øÀ½
		}
		else
		{
			logStr.Format("Lens ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -31000;
		}
		break;
	case 31500:
		if (motor.Lens_Motor_MoveY(Loading_Pos))
		{
			iRtnFunction = 32000;		//·»ÁîÂÊ Ä«¸Þ¶ó ¾øÀ½
		}
		else
		{
			logStr.Format("Lens LensLoad ÀÌµ¿ ½ÇÆÐ[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -31500;
		}
		break;
	case 32000:
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,TyÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -32000;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(1, Loading_Pos))
		{
			logStr.Format("Lens %s X,Y ÀÌµ¿ ½ÇÆÐ", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -32000;
			break;
		}

		logStr.Format("Lens °ø±Þ À§Ä¡ ÀÌµ¿ ¿Ï·á[%d]", iLensStep);
		putListLog(logStr);
		iRtnFunction = 32500;
		break;
	case 32500:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ÇØÁ¦ ½ÇÆÐ[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -32500;
			break;
		}
		Task.LensTaskTime = myTimer(true);
		iRtnFunction = 33000;
		break;
	case 33000:
		if (Dio.LensMotorGripCheck(false, false))
		{
			iRtnFunction = 33500;
			Task.LensTaskTime = myTimer(true);
		}
		else if ((myTimer(true) - Task.LensTaskTime) > IO_DELAY_TIME)
		{
			iRtnFunction = -32500;
		}
		break;
	case 33500:
		if (!motor.Lens_LaserAlign_Motor_MoveZ(Loading_Pos))
		{
			logStr.Format("LensLaserAlign loading Z ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -33500;
			break;
		}
		iRtnFunction = 34000;
		break;
	case 34000:
		if (!motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens Z À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -34000;
			break;
		}
		Sleep(300);
		iRtnFunction = 34500;
		break;
	case 34500:
		Dio.LensMotorGrip(true, false);
		Sleep(500);
		iRtnFunction = 35000;
		break;
	case 35000:
		Dio.LensMotorGrip(false, false);
		Sleep(300);
		iRtnFunction = 35500;
		break;
	case 35500:
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 36000;
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens ±×¸³ ½ÇÆÐ [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35500;
		}
		break;
	case 36000:
		if (Dio.LensMotorGripCheck(true, false))
		{
			logStr.Format("Lens ±×¸³ ¿Ï·á[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 36200;
		}
		else if ((myTimer(true) - Task.LensTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ÀüÁø ¼¾¼­ ½Ã°£ ÃÊ°ú..[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35500;
		}
		break;
	case 36200:
		if (LGIT_MODEL_INDEX == M2_OV9284_5M || LGIT_MODEL_INDEX == M2_VB1940_5M)
		{
			Dio.LensVacuumOn(false);
		}
		iRtnFunction = 36300;
		break;
	case 36300:
		
		iRtnFunction = 36400;
		break;
	case 36400:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ´ë±â À§Ä¡ ÀÌµ¿ ½ÇÆÐ [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -36400;
			break;
		}
		iRtnFunction = 36600;
		break;
	case 36600:
		if (!motor.Lens_LaserAlign_Motor_MoveZ(Wait_Pos))
		{
			logStr.Format("LensLaserAlign waiting Z ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -36600;
			break;
		}
		iRtnFunction = 36800;
		break;
	case 36800:
		if (!motor.Lens_Motor_MoveX(Wait_Pos))
		{
			logStr.Format("Lens X ´ë±âÀ§Ä¡ ÀÌµ¿ ½ÇÆÐ");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -36800;
			break;
		}
		iRtnFunction = 37000;
		break;
	case 37000:
		iRtnFunction = 37200;
		break;
	case 37200:
		iRtnFunction = 37400;
		break;
	case 37400:
		iRtnFunction = 39500;
		break;
	case 39500:
		Task.interlockLens = 1;		//Lens AA »ç¿ë 2È£±â, Lens Thread , ·»Áî ·Îµå ¿Ï·á ºÎºÐ
		iRtnFunction = 50000;
		break;
	default:
		logStr.Format("Lens Load Thread Step ¹øÈ£ ºñÁ¤»ó [%d]", iLensStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}


