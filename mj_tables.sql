-- --------------------------------------------------------
-- 主机:                           116.62.59.105
-- 服务器版本:                        5.7.22-log - MySQL Community Server (GPL)
-- 服务器操作系统:                      Linux
-- HeidiSQL 版本:                  9.5.0.5280
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


-- 导出 mj_center 的数据库结构
CREATE DATABASE IF NOT EXISTS `mj_center` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `mj_center`;

-- 导出  表 mj_center.charge 结构
CREATE TABLE IF NOT EXISTS `charge` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Time` int(11) DEFAULT NULL,
  `CardType` int(11) DEFAULT NULL COMMENT '牌局类型，多少局，可能无用',
  `CardNum` int(11) DEFAULT NULL COMMENT '钻石数量',
  `Type` int(11) DEFAULT NULL COMMENT '参考CARDS_OPER_TYPE',
  `Admin` varchar(50) DEFAULT '',
  `UserId` int(11) DEFAULT NULL,
  `TotalCardNum` int(11) DEFAULT '0',
  `Remark` varchar(256) DEFAULT NULL,
  PRIMARY KEY (`Id`),
  KEY `UserId` (`UserId`),
  KEY `Time` (`Time`),
  KEY `Admin` (`Admin`),
  KEY `Type` (`Type`),
  KEY `type_time` (`Type`,`Time`)
) ENGINE=InnoDB AUTO_INCREMENT=758 DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_center.gonghui 结构
CREATE TABLE IF NOT EXISTS `gonghui` (
  `id` int(11) unsigned NOT NULL,
  `name` varchar(64) COLLATE utf8mb4_bin NOT NULL,
  `detail` varchar(64) COLLATE utf8mb4_bin NOT NULL DEFAULT '',
  `adminId` int(11) unsigned NOT NULL,
  `roomPolicy` varchar(256) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '房间策略(几人麻将,底分,局数,最低几个房间)：3,1,10,1;4,2,5,2',
  `status` varchar(256) COLLATE utf8mb4_bin NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

-- 数据导出被取消选择。
-- 导出  表 mj_center.onlinelog 结构
CREATE TABLE IF NOT EXISTS `onlinelog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `DateTime` int(11) NOT NULL,
  `serverID` int(11) NOT NULL DEFAULT '0',
  `serverName` varchar(128) NOT NULL DEFAULT '',
  `onlineCount` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `DateTime` (`DateTime`)
) ENGINE=InnoDB AUTO_INCREMENT=392701 DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_center.order 结构
CREATE TABLE IF NOT EXISTS `order` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `UserId` int(11) NOT NULL DEFAULT '0' COMMENT 'ç”¨æˆ·ID',
  `CreateTime` int(11) NOT NULL DEFAULT '0' COMMENT 'åˆ›å»ºæ—¶é—´',
  `ReturnTime` int(11) NOT NULL DEFAULT '0' COMMENT 'åˆ°è´¦æ—¶é—´',
  `Status` smallint(1) NOT NULL DEFAULT '0' COMMENT 'çŠ¶æ€: 0åˆ é™¤ï¼Œ 1æˆåŠŸï¼Œ 2å¤±è´¥ï¼Œ3è¿›è¡Œä¸­',
  `OtherNo` varchar(40) NOT NULL DEFAULT '0' COMMENT 'ç¬¬ä¸‰æ–¹è®¢å•å·',
  `BeforeMoney` bigint(20) NOT NULL DEFAULT '0' COMMENT 'å……å€¼å‰é‡‘é¢',
  `Money` int(11) NOT NULL DEFAULT '0' COMMENT 'é‡‘é¢',
  `RealMoney` int(11) NOT NULL DEFAULT '0' COMMENT 'å®žé™…é‡‘é¢',
  `Method` smallint(1) NOT NULL DEFAULT '0' COMMENT 'å……å€¼æ–¹å¼: 1æ”¯ä»˜å®ï¼Œ2å¾®ä¿¡ï¼Œ 3è‹¹æžœ,  4å…¬å¸æ”¯ä»˜å®ï¼Œ5å…¬å¸å¾®ä¿¡',
  `GameId` int(11) NOT NULL DEFAULT '0' COMMENT 'æ¸¸æˆID',
  `ChannelNo` int(11) NOT NULL DEFAULT '0' COMMENT 'æ¸ é“å·',
  `Remark` varchar(255) NOT NULL DEFAULT '' COMMENT 'å¤‡æ³¨',
  `CreateDateTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ReturnDateTime` datetime DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=500040174 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='è®¢å•';

-- 数据导出被取消选择。
-- 导出  表 mj_center.room 结构
CREATE TABLE IF NOT EXISTS `room` (
  `roomId` int(11) NOT NULL,
  `gonghuiId` int(11) NOT NULL,
  `baseScore` int(11) NOT NULL COMMENT '桌子底分',
  `cardType` int(11) NOT NULL COMMENT '桌子可以打的局数',
  `playType` int(11) NOT NULL COMMENT '3人麻将还是4人麻将',
  `playNums` int(11) NOT NULL COMMENT '打了多少局',
  `deskState` varchar(64) NOT NULL DEFAULT '' COMMENT '结算时房间状态',
  `user1` int(11) NOT NULL,
  `user2` int(11) NOT NULL,
  `user3` int(11) NOT NULL,
  `user4` int(11) NOT NULL,
  `score1` int(11) NOT NULL COMMENT '用户1的分数',
  `score2` int(11) NOT NULL,
  `score3` int(11) NOT NULL,
  `score4` int(11) NOT NULL,
  `lastModify` int(11) NOT NULL COMMENT '最后修改时间',
  PRIMARY KEY (`roomId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_center.r_gonghui_apply 结构
CREATE TABLE IF NOT EXISTS `r_gonghui_apply` (
  `gonghuiId` int(11) NOT NULL,
  `userId` int(11) NOT NULL,
  `opType` int(11) NOT NULL COMMENT '1表示申请加入工会',
  `opStatus` varchar(50) NOT NULL COMMENT 'Apply、Refused、Approved分别表示申请，同意和拒绝',
  UNIQUE KEY `gonghuiApply` (`gonghuiId`,`userId`,`opType`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_center.r_gonghui_user 结构
CREATE TABLE IF NOT EXISTS `r_gonghui_user` (
  `gonghuiId` int(11) NOT NULL,
  `userId` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_center.serverlist 结构
CREATE TABLE IF NOT EXISTS `serverlist` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Ip` varchar(50) DEFAULT '0',
  `Port` int(11) DEFAULT '0',
  `Online` int(11) DEFAULT '0',
  `GroupId` int(11) DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_center.taskinfo 结构
CREATE TABLE IF NOT EXISTS `taskinfo` (
  `id` int(11) NOT NULL COMMENT '任务ID',
  `type` int(11) NOT NULL COMMENT '任务类型',
  `gonghuiId` int(11) NOT NULL COMMENT '申请加入的工会ID',
  `userId` int(11) NOT NULL COMMENT '申请人',
  `status` int(11) NOT NULL COMMENT '当前任务状态',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_center.user 结构
CREATE TABLE IF NOT EXISTS `user` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `OpenId` varchar(64) COLLATE utf8mb4_bin NOT NULL,
  `Nike` varchar(64) COLLATE utf8mb4_bin NOT NULL,
  `Sex` int(11) DEFAULT '0',
  `Provice` varchar(64) COLLATE utf8mb4_bin DEFAULT '',
  `City` varchar(64) COLLATE utf8mb4_bin DEFAULT '',
  `Country` varchar(64) COLLATE utf8mb4_bin DEFAULT '',
  `HeadImageUrl` varchar(512) COLLATE utf8mb4_bin DEFAULT '',
  `UnionId` varchar(64) COLLATE utf8mb4_bin NOT NULL DEFAULT '',
  `NumsCard1` int(11) DEFAULT '0',
  `NumsCard2` int(11) unsigned DEFAULT '0',
  `NumsCard3` int(11) DEFAULT '0',
  `LastLoginTime` int(11) DEFAULT '0',
  `LastLoginIP` varchar(15) COLLATE utf8mb4_bin DEFAULT '',
  `RegTime` int(11) DEFAULT '0',
  `RegIP` varchar(15) COLLATE utf8mb4_bin DEFAULT '',
  `New` int(11) DEFAULT '0',
  `Gm` int(11) DEFAULT '0',
  `TotalCardNum` int(11) DEFAULT '0',
  `TotalPlayNum` int(11) DEFAULT '0',
  `Coins` int(11) DEFAULT '0',
  `Credits` int(11) DEFAULT '0',
  `AgentPower` int(11) DEFAULT '0',
  `AgentId` int(11) DEFAULT '0',
  `Status` int(6) DEFAULT '1',
  PRIMARY KEY (`Id`),
  KEY `NumsCard1` (`NumsCard1`),
  KEY `NumsCard2` (`NumsCard2`),
  KEY `NumsCard3` (`NumsCard3`),
  KEY `LastLoginTime` (`LastLoginTime`),
  KEY `LastLoginIP` (`LastLoginIP`),
  KEY `RegTime` (`RegTime`),
  KEY `RegIP` (`RegIP`),
  KEY `OpenId` (`OpenId`),
  KEY `UnionId` (`UnionId`),
  KEY `Coins` (`Coins`),
  KEY `Credits` (`Credits`),
  KEY `AgentPower` (`AgentPower`),
  KEY `AgentId` (`AgentId`),
  KEY `Status` (`Status`)
) ENGINE=InnoDB AUTO_INCREMENT=497 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

-- 数据导出被取消选择。
-- 导出  表 mj_center.webOpLog 结构
CREATE TABLE IF NOT EXISTS `webOpLog` (
  `id` int(32) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
  `userId` int(32) NOT NULL COMMENT '用户ID',
  `opType` int(16) NOT NULL COMMENT '操作类型 1:加钻  2:创建工会',
  `remark1` int(16) DEFAULT NULL COMMENT 'opType为1：添加钻石数   opType为2：创建工会ID',
  `remark2` int(32) DEFAULT NULL COMMENT 'opType为1：添加前钻石数',
  `remark3` int(16) DEFAULT NULL COMMENT 'opType为1：对应金额',
  `remark4` varchar(128) DEFAULT NULL COMMENT '备注',
  `opTime` datetime DEFAULT NULL COMMENT '操作时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=51 DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。

-- 导出 mj_logic 的数据库结构
CREATE DATABASE IF NOT EXISTS `mj_logic` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `mj_logic`;

-- 导出  表 mj_logic.activelog 结构
CREATE TABLE IF NOT EXISTS `activelog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `activeID` int(11) NOT NULL,
  `userID` int(11) NOT NULL,
  `phone` varchar(20) NOT NULL DEFAULT '',
  `reward` int(11) NOT NULL,
  `date` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `activeID` (`activeID`),
  KEY `userID` (`userID`),
  KEY `date` (`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_logic.activitylog 结构
CREATE TABLE IF NOT EXISTS `activitylog` (
  `AutoId` int(11) NOT NULL AUTO_INCREMENT,
  `Id` int(11) NOT NULL,
  `UserId` int(11) NOT NULL DEFAULT '0',
  `SpendType` int(11) NOT NULL DEFAULT '0',
  `SpendCount` int(11) NOT NULL DEFAULT '0',
  `GiftName` varchar(128) NOT NULL DEFAULT '',
  `GiftType` int(11) NOT NULL DEFAULT '0',
  `GiftCount` int(11) NOT NULL DEFAULT '0',
  `Date` int(11) NOT NULL DEFAULT '0',
  `Phone` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`AutoId`),
  KEY `Id` (`Id`),
  KEY `UserId` (`UserId`),
  KEY `Date` (`Date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_logic.bindingrelationships 结构
CREATE TABLE IF NOT EXISTS `bindingrelationships` (
  `Invitee` int(11) NOT NULL,
  `Inviter` int(11) NOT NULL,
  `BindingTime` int(11) DEFAULT NULL,
  `TaskFinished` int(11) DEFAULT NULL,
  PRIMARY KEY (`Invitee`,`Inviter`),
  KEY `Inviter` (`Inviter`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 数据导出被取消选择。
-- 导出  表 mj_logic.log 结构
CREATE TABLE IF NOT EXISTS `log` (
  `Id` varchar(64) NOT NULL,
  `Time` int(11) DEFAULT '0',
  `Pos1` int(11) DEFAULT '0',
  `Pos2` int(11) DEFAULT '0',
  `Pos3` int(11) DEFAULT '0',
  `Pos4` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `DeskId` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `Pass` varchar(50) DEFAULT '',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Reset` int(11) DEFAULT '0',
  `Data` varchar(2048) DEFAULT '',
  `PlayType` varchar(64) DEFAULT '',
  `checkTing1` int(11) DEFAULT NULL,
  `checkTing2` int(11) DEFAULT NULL,
  `checkTing3` int(11) DEFAULT NULL,
  `checkTing4` int(11) DEFAULT NULL,
  PRIMARY KEY (`Id`),
  KEY `Pos1` (`Pos1`),
  KEY `Pos2` (`Pos2`),
  KEY `Time` (`Time`),
  KEY `Pos3` (`Pos3`),
  KEY `Pos4` (`Pos4`),
  KEY `MaxCircle` (`MaxCircle`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_logic.logcoins 结构
CREATE TABLE IF NOT EXISTS `logcoins` (
  `Id` varchar(64) NOT NULL,
  `Time` int(11) DEFAULT '0',
  `Pos1` int(11) DEFAULT '0',
  `Pos2` int(11) DEFAULT '0',
  `Pos3` int(11) DEFAULT '0',
  `Pos4` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `DeskId` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `Pass` varchar(50) DEFAULT '',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Reset` int(11) DEFAULT '0',
  `Data` varchar(2048) DEFAULT '',
  `PlayType` varchar(64) DEFAULT '',
  `checkTing1` int(11) DEFAULT NULL,
  `checkTing2` int(11) DEFAULT NULL,
  `checkTing3` int(11) DEFAULT NULL,
  `checkTing4` int(11) DEFAULT NULL,
  PRIMARY KEY (`Id`),
  KEY `Pos1` (`Pos1`),
  KEY `Pos2` (`Pos2`),
  KEY `Time` (`Time`),
  KEY `Pos3` (`Pos3`),
  KEY `Pos4` (`Pos4`),
  KEY `MaxCircle` (`MaxCircle`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_logic.video 结构
CREATE TABLE IF NOT EXISTS `video` (
  `Id` varchar(64) NOT NULL,
  `UserId1` int(11) DEFAULT NULL,
  `Time` int(11) DEFAULT '0',
  `UserId2` int(11) DEFAULT NULL,
  `UserId3` int(11) DEFAULT NULL,
  `UserId4` int(11) DEFAULT NULL,
  `Data` varchar(4096) DEFAULT NULL,
  `DeskId` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Zhuang` int(11) DEFAULT '0',
  `PlayType` varchar(64) DEFAULT '',
  PRIMARY KEY (`Id`),
  KEY `Time` (`Time`),
  KEY `UserId1` (`UserId1`),
  KEY `UserId2` (`UserId2`),
  KEY `UserId3` (`UserId3`),
  KEY `UserId4` (`UserId4`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_logic.videocoins 结构
CREATE TABLE IF NOT EXISTS `videocoins` (
  `Id` varchar(64) NOT NULL,
  `UserId1` int(11) DEFAULT NULL,
  `Time` int(11) DEFAULT '0',
  `UserId2` int(11) DEFAULT NULL,
  `UserId3` int(11) DEFAULT NULL,
  `UserId4` int(11) DEFAULT NULL,
  `Data` varchar(4096) DEFAULT NULL,
  `DeskId` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Zhuang` int(11) DEFAULT '0',
  `PlayType` varchar(64) DEFAULT '',
  PRIMARY KEY (`Id`),
  KEY `Time` (`Time`),
  KEY `UserId1` (`UserId1`),
  KEY `UserId2` (`UserId2`),
  KEY `UserId3` (`UserId3`),
  KEY `UserId4` (`UserId4`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
-- 导出  表 mj_logic.WEB_USER 结构
CREATE TABLE IF NOT EXISTS `WEB_USER` (
  `USERNAME` char(32) NOT NULL COMMENT '用户名',
  `PASSWORD` char(32) NOT NULL COMMENT '密码MD5',
  `USER_GROUP` int(32) NOT NULL DEFAULT '1' COMMENT '用户组,1.管理后台用户',
  `USER_PERMISSION` int(32) NOT NULL DEFAULT '1' COMMENT '用户权限,1:数据查询，2：数据操作',
  PRIMARY KEY (`USERNAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
