#include "LVipLog.h"

#define R00M_INIT_SCORE 0

LVipLogItem::LVipLogItem() : m_id("")
{
	m_time = 0;
	m_state = 0;	//
	m_deskId = 0;		//底注
	m_maxCircle = 0;//总圈数
	m_curCircle = 0;//当前圈数
	memset(m_posUserId, 0, sizeof(m_posUserId));
	m_curZhuangPos = 0;
	m_checkTing[0] = 0;
	m_checkTing[1] = 0;
	m_checkTing[2] = 0;
	m_checkTing[3] = 0;
	m_score[0] = R00M_INIT_SCORE;//1000;
	m_score[1] = R00M_INIT_SCORE;//1000;
	m_score[2] = R00M_INIT_SCORE;//1000;
	m_score[3] = R00M_INIT_SCORE;//1000;
	m_reset = 0;
	m_playtype.clear();
	m_iPlayerCapacity = DESK_USER_COUNT;
}


LVipLogItem::~LVipLogItem()
{
	for(Lsize i = 0 ; i < m_log.size(); ++i)
	{
		delete m_log[i];
	}
}

Lstring LVipLogItem::ToString()
{
	std::stringstream ss;

	//for (Lsize i = 0; i < m_log.size(); ++i)
	//{
	//	ss << m_log[i]->m_gold[0] << "," << m_log[i]->m_gold[1] << "," << m_log[i]->m_gold[2] << "," << m_log[i]->m_gold[3] << ";"; // 0
	//	ss << m_log[i]->m_mgang[0] << "," << m_log[i]->m_mgang[1] << "," << m_log[i]->m_mgang[2] << "," << m_log[i]->m_mgang[3] << ";"; // 1
	//	ss << m_log[i]->m_angang[0] << "," << m_log[i]->m_angang[1] << "," << m_log[i]->m_angang[2] << "," << m_log[i]->m_angang[3] << ";"; // 2
	//	ss << m_log[i]->m_zhuangPos << ";"; // 3
	//	ss << m_log[i]->m_time << ";"; // 4
	//	ss << m_log[i]->m_videoId <<";"; // 5
	//	ss << m_log[i]->m_checkTing[0] << "," << m_log[i]->m_checkTing[1] << "," << m_log[i]->m_checkTing[2] << "," << m_log[i]->m_checkTing[3] << ";"; // 6
	//	ss << m_log[i]->m_zimo[0] << "," << m_log[i]->m_zimo[1] << "," << m_log[i]->m_zimo[2] << "," << m_log[i]->m_zimo[3] << ";"; // 7
	//	ss << m_log[i]->m_bomb[0] << "," << m_log[i]->m_bomb[1] << "," << m_log[i]->m_bomb[2] << "," << m_log[i]->m_bomb[3] << ";"; // 8
	//	ss << m_log[i]->m_win[0] << "," << m_log[i]->m_win[1] << "," << m_log[i]->m_win[2] << "," << m_log[i]->m_win[3] << ";"; // 9
	//	ss << m_log[i]->m_dzimo[0] << "," << m_log[i]->m_dzimo[1] << "," << m_log[i]->m_dzimo[2] << "," << m_log[i]->m_dzimo[3] << ";"; // 10
	//	ss << m_log[i]->m_dbomb[0] << "," << m_log[i]->m_dbomb[1] << "," << m_log[i]->m_dbomb[2] << "," << m_log[i]->m_dbomb[3] << ";"; // 11
	//	ss << m_log[i]->m_dwin[0] << "," << m_log[i]->m_dwin[1] << "," << m_log[i]->m_dwin[2] << "," << m_log[i]->m_dwin[3] << ";"; // 12
	//	ss << "|";
	//}
	for (Lsize i = 0; i < m_log.size(); ++i)
	{
		ss << m_log[i]->m_gold[0] << "," << m_log[i]->m_gold[1] << "," << m_log[i]->m_gold[2] << "," << m_log[i]->m_gold[3] << ",";
		ss << m_log[i]->m_win[0] << "," << m_log[i]->m_win[1] << "," << m_log[i]->m_win[2] << "," << m_log[i]->m_win[3] << ",";
		ss << m_log[i]->m_mgang[0] << "," << m_log[i]->m_mgang[1] << "," << m_log[i]->m_mgang[2] << "," << m_log[i]->m_mgang[3] << ",";
		ss << m_log[i]->m_angang[0] << "," << m_log[i]->m_angang[1] << "," << m_log[i]->m_angang[2] << "," << m_log[i]->m_angang[3] << ",";
		ss << m_log[i]->m_bomb[0] << "," << m_log[i]->m_bomb[1] << "," << m_log[i]->m_bomb[2] << "," << m_log[i]->m_bomb[3] << ",";
		ss << m_log[i]->m_zhuangPos << "," << m_log[i]->m_time << "," << m_log[i]->m_videoId <<";";
	}
	return ss.str();
}

void LVipLogItem::FromString(const Lstring& str)
{
	std::vector<Lstring> str1, str2;

	L_ParseString(str, str2, ";");

	for(Luint j = 0 ; j < str2.size(); ++j)
	{
		str1.clear();
		L_ParseString(str2[j], str1, ",");
		if (str1.size() == 23)
		{
			VipDeskLog* log = new VipDeskLog();
			Lint i = 0;
			for (; i < DESK_USER_COUNT; ++i)
			{
				log->m_gold[i] = atoi(str1[i].c_str());
				log->m_win[i] = atoi(str1[i+4].c_str());
				log->m_mgang[i] = atoi(str1[i+8].c_str());
				log->m_angang[i] = atoi(str1[i+12].c_str());
				log->m_bomb[i] = atoi(str1[i+16].c_str());
			}

			log->m_zhuangPos = atoi(str1[20].c_str());
			log->m_time = atoi(str1[21].c_str());
			log->m_videoId = str1[22].c_str();
			m_log.push_back(log);
		}
	}
	
	//if ( str.find( "|" ) != std::string::npos )
	//{
	//	L_ParseString(str, str2, "|");
	//	for(size_t j = 0 ; j < str2.size(); ++j)
	//	{
	//		str1.clear();
	//		L_ParseString(str2[j], str1, ";");
	//		if ( str1.size() >= 8 )
	//		{
	//			VipDeskLog* log = new VipDeskLog();

	//			sscanf( str1[0].c_str(), "%d,%d,%d,%d", &(log->m_gold[0]), &(log->m_gold[1]), &(log->m_gold[2]), &(log->m_gold[3]) );
	//			sscanf( str1[1].c_str(), "%d,%d,%d,%d", &(log->m_mgang[0]), &(log->m_mgang[1]), &(log->m_mgang[2]), &(log->m_mgang[3]) );
	//			sscanf( str1[2].c_str(), "%d,%d,%d,%d", &(log->m_angang[0]), &(log->m_angang[1]), &(log->m_angang[2]), &(log->m_angang[3]) );
	//			log->m_zhuangPos = atoi(str1[3].c_str());
	//			log->m_time = atoi(str1[4].c_str());
	//			log->m_videoId = str1[5].c_str();
	//			sscanf( str1[6].c_str(), "%d,%d,%d,%d", &(log->m_checkTing[0]), &(log->m_checkTing[1]), &(log->m_checkTing[2]), &(log->m_checkTing[3]) );
	//			sscanf( str1[7].c_str(), "%d,%d,%d,%d", &(log->m_zimo[0]), &(log->m_zimo[1]), &(log->m_zimo[2]), &(log->m_zimo[3]) );
	//			sscanf( str1[8].c_str(), "%d,%d,%d,%d", &(log->m_bomb[0]), &(log->m_bomb[1]), &(log->m_bomb[2]), &(log->m_bomb[3]) );
	//			sscanf( str1[9].c_str(), "%d,%d,%d,%d", &(log->m_win[0]), &(log->m_win[1]), &(log->m_win[2]), &(log->m_win[3]) );
	//			sscanf( str1[10].c_str(), "%d,%d,%d,%d", &(log->m_dzimo[0]), &(log->m_dzimo[1]), &(log->m_dzimo[2]), &(log->m_dzimo[3]) );
	//			sscanf( str1[11].c_str(), "%d,%d,%d,%d", &(log->m_dbomb[0]), &(log->m_dbomb[1]), &(log->m_dbomb[2]), &(log->m_dbomb[3]) );
	//			sscanf( str1[12].c_str(), "%d,%d,%d,%d", &(log->m_dwin[0]), &(log->m_dwin[1]), &(log->m_dwin[2]), &(log->m_dwin[3]) );
	//			m_log.push_back(log);
	//		}
	//	}
	//}
	//else
	//{
	//	// 之前老的结构数据 新版本的程序上线24小时候 这个逻辑就不会再走到了
	//	L_ParseString(str, str2, ";");
	//	for(size_t j = 0 ; j < str2.size(); ++j)
	//	{
	//		str1.clear();
	//		L_ParseString(str2[j], str1, ",");
	//		if (str1.size() >= 27)
	//		{
	//			VipDeskLog* log = new VipDeskLog();
	//			Lint i = 0;
	//			for (; i < DESK_USER_COUNT; ++i)
	//			{
	//				log->m_gold[i] = atoi(str1[i].c_str());
	//				int hutype = atoi(str1[i+4].c_str());
	//				if ( hutype == WIN_SUB_ZIMO )
	//					log->m_zimo[i] = 1;
	//				else if ( hutype == WIN_SUB_BOMB )
	//					log->m_win[i] = 1;
	//				else if ( hutype == WIN_SUB_ABOMB )
	//					log->m_bomb[i] = 1;
	//				log->m_mgang[i] = atoi(str1[i+8].c_str());
	//				log->m_angang[i] = atoi(str1[i+12].c_str());
	//				log->m_bomb[i] = atoi(str1[i+16].c_str());
	//			}
	//			log->m_zhuangPos = atoi(str1[20].c_str());
	//			log->m_time = atoi(str1[21].c_str());
	//			log->m_videoId = str1[22].c_str();
	//			for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	//			{
	//				log->m_checkTing[i] = atoi(str1[23+i].c_str());
	//			}
	//			m_log.push_back(log);
	//		}
	//	}
	//}
}

Lstring LVipLogItem::PlayTypeToStrint()
{
	std::stringstream ss;

	for (Lsize i = 0; i < m_playtype.size(); ++i)
	{
		ss << m_playtype[i] << ";";
	}

	return ss.str();
}

void LVipLogItem::PlayTypeFromString(const Lstring& str)
{
	std::vector<Lstring> strSplid;

	L_ParseString(str, strSplid, ";");
	if (strSplid.size() > 0)
	{
		for(size_t j = 0 ; j < strSplid.size(); ++j)
		{
			m_playtype.push_back( atoi(strSplid[j].c_str()));
		}
	}
}
