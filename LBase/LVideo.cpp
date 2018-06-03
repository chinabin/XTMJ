#include "LVideo.h"
#include "LTime.h"

std::ostream& operator<<(std::ostream& ss, const VideoOper& vo)
{
	ss << vo.m_pos << ",";
	ss << vo.m_oper << ",";
	for (size_t i = 0; i< vo.m_cards.size(); i++)
	{
		ss << vo.m_cards[i].m_color << "|" << vo.m_cards[i].m_number;
		if (i < vo.m_cards.size() - 1)
		{
			ss << ",";
		}
	}
	ss << ";";
	return ss;
}

VideoLog::VideoLog() : m_Id("")
{
	memset(m_userId, 0, sizeof(m_userId));
	LTime stTime;
	m_time = stTime.Secs();
	m_curCircle = 0;
	m_maxCircle = 0;
	m_deskId = 0;
	memset(m_score,0,sizeof(m_score));
	m_zhuang = 0;
	m_flag = 0;
}

VideoLog::~VideoLog()
{

}

void VideoLog::AddOper(Lint oper, Lint pos, std::vector<CardValue>& cards)
{
	VideoOper o;
	o.m_oper = oper;
	o.m_pos = pos;
// 	for (int i = 0; i < cards.size(); ++i)
// 	{
// 		o.m_cards.push_back(cards[i]);
// 	}
	o.m_cards = cards;
	m_oper.push_back(o);
}


void VideoLog::DealCard(Lint* userId, std::vector<CardValue>* vec,Lint time,Lint zhuang,Lint* score,Lint deskId,Lint cur,Lint max,Lint flag, const std::vector<Lint>& playType)
{
	memcpy(m_userId, userId, sizeof(m_userId));
	memcpy(m_score,score,sizeof(m_score));
	for (Lint i = 0; i < 4; ++i)
	{
		m_handCard[i] = vec[i];
	}
	m_time = time;
	m_deskId = deskId;
	m_maxCircle = max;
	m_curCircle = cur;
	m_zhuang = zhuang;
	m_flag = flag;
// 	auto itype = playType.begin();
// 	for (; itype != playType.end(); ++itype)
// 	{
// 		m_playType.push_back( *itype );
// 	}
	m_playType = playType;
}

Lstring	VideoLog::ToString()
{
	std::stringstream ss;

	for (Lint i = 0; i < 4; ++i)
	{
		for (Lint j = 0; j < m_handCard[i].size(); ++j)
		{
			ss << m_handCard[i][j].m_color << "," << m_handCard[i][j].m_number;
			if (j + 1 != m_handCard[i].size())
			{
				ss << ",";
			}
		}
		ss << ";";
	}

	for (Lint i = 0; i < m_oper.size(); ++i)
	{
		ss << m_oper[i];//m_oper[i].ToString();
	}
	return ss.str();
}

//重载实现了VideoLog::ToString()的功能
std::ostream& operator<<(std::ostream& ss, const VideoLog& vl)
{
	for (Lint i = 0; i < 4; ++i)
	{
		for (Lint j = 0; j < vl.m_handCard[i].size(); ++j)
		{
			ss << vl.m_handCard[i][j].m_color << "," << vl.m_handCard[i][j].m_number;
			if (j + 1 != vl.m_handCard[i].size())
			{
				ss << ",";
			}
		}
		ss << ";";
	}

	for (Lint i = 0; i < vl.m_oper.size(); ++i)
	{
		ss << vl.m_oper[i];//m_oper[i].ToString();
	}
	return ss;
}

void VideoLog::FromString(Lstring str)
{
	std::vector<Lstring> des,subDes;
	L_ParseString(str, des, ";");
	if (des.size() >= 4)
	{
		for (Lint i = 0; i < 4; ++i)
		{
			subDes.clear();
			L_ParseString(des[i], subDes, ",");
			for (Lint j = 0; j+1 < subDes.size(); j+=2)
			{
				CardValue v;
				v.m_color = atoi(subDes[j].c_str());
				v.m_number = atoi(subDes[j+1].c_str());
				m_handCard[i].push_back(v);
			}
		}

		for (Lint i = 4; i < des.size(); ++i)
		{
			VideoOper o;
			o.FromString(des[i]);
			if (o.m_oper != 0)
			{
				m_oper.push_back(o);
			}
		}
	}
}

void VideoLog::Write(LBuff& buff)
{
	buff.Write(m_Id);
	buff.Write(m_time);
	buff.Write(m_userId[0]);
	buff.Write(m_userId[1]);
	buff.Write(m_userId[2]);
	buff.Write(m_userId[3]);
	buff.Write(m_zhuang);
	buff.Write(m_deskId);
	buff.Write(m_curCircle);
	buff.Write(m_maxCircle);
	buff.Write(m_score[0]);
	buff.Write(m_score[1]);
	buff.Write(m_score[2]);
	buff.Write(m_score[3]);
	buff.Write(m_flag);

	buff.Write(m_str);
	buff.Write(m_str_playtype);
}

void VideoLog::Read(LBuff& buff)
{
	buff.Read(m_Id);
	buff.Read(m_time);
	buff.Read(m_userId[0]);
	buff.Read(m_userId[1]);
	buff.Read(m_userId[2]);
	buff.Read(m_userId[3]);
	buff.Read(m_zhuang);
	buff.Read(m_deskId);
	buff.Read(m_curCircle);
	buff.Read(m_maxCircle);
	buff.Read(m_score[0]);
	buff.Read(m_score[1]);
	buff.Read(m_score[2]);
	buff.Read(m_score[3]);
	buff.Read(m_flag);
	m_str.clear();
	buff.Read(m_str);
	m_str_playtype.clear();
	buff.Read(m_str_playtype);
}

void VideoLog::Clear()
{
	m_time = 0;
	m_Id = "";
	memset(m_userId, 0, sizeof(m_userId));
	for (Lint i = 0; i < 4; ++i)
	{
		m_handCard[i].clear();
	}
	m_oper.clear();
	m_str.clear();
	m_str_playtype.clear();
}

Lstring VideoLog::GetInsertSql()
{
	std::stringstream ss;
	ss << "INSERT INTO";
	if ( m_flag > 1000 )	// > 1000 存放到金币表中
	{
		ss << " videocoins ";
	}
	else
	{
		ss << " video ";
	}
	ss << "(Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data, PlayType) VALUES (";
	ss << "'" << m_Id << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_userId[0] << "',";
	ss << "'" << m_userId[1] << "',";
	ss << "'" << m_userId[2] << "',";
	ss << "'" << m_userId[3] << "',";

	ss << "'" << m_zhuang << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_score[0] << "',";
	ss << "'" << m_score[1] << "',";
	ss << "'" << m_score[2] << "',";
	ss << "'" << m_score[3] << "',";
	ss << "'" << m_flag << "',";
	ss << "'" << /*ToString()*/ *this << "',";
	ss << "'" << PlayTypeToStrint() << "')";
	return ss.str();
}

Lstring VideoLog::PlayTypeToStrint()
{
	std::stringstream ss;

	for (Lint i = 0; i < m_playType.size(); ++i)
	{
		ss << m_playType[i] <<";";
	}
	return ss.str();
}

void VideoLog::PlayTypeFromString(const Lstring& str)
{
	std::vector<Lstring> des;
	L_ParseString(str, des, ";");
	if (des.size() > 0)
	{
		for(Lint j = 0 ; j < des.size(); ++j)
		{
			m_playType.push_back( atoi(des[j].c_str()));
		}
	}
}

