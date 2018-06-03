-- --------------------------------------------------------
-- 主机:                           rm-bp1o1318c5id74c7i.mysql.rds.aliyuncs.com
-- 服务器版本:                        5.6.29 - Source distribution
-- 服务器操作系统:                      Linux
-- HeidiSQL 版本:                  8.2.0.4675
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- 导出 sichuan 的数据库结构
CREATE DATABASE IF NOT EXISTS `mj_center` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `mj_center`;


-- 导出  表 sichuan.charge 结构
CREATE TABLE IF NOT EXISTS `charge` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Time` int(11) DEFAULT NULL,
  `CardType` int(11) DEFAULT NULL,
  `CardNum` int(11) DEFAULT NULL,
  `Type` int(11) DEFAULT NULL,
  `Admin` varchar(50) DEFAULT '',
  `UserId` int(11) DEFAULT NULL,
  `TotalCardNum` int(11) DEFAULT '0',
  PRIMARY KEY (`Id`),
  KEY `UserId` (`UserId`),
  KEY `Time` (`Time`),
  KEY `Admin` (`Admin`),
  KEY `Type` (`Type`),
  KEY `type_time` (`Type`,`Time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。


-- 导出  表 sichuan.onlinelog 结构
CREATE TABLE IF NOT EXISTS `onlinelog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `DateTime` int(11) NOT NULL,
  `serverID` int(11) NOT NULL DEFAULT '0',
  `serverName` varchar(128) NOT NULL DEFAULT '',
  `onlineCount` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `DateTime` (`DateTime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。


-- 导出  表 sichuan.serverlist 结构
CREATE TABLE IF NOT EXISTS `serverlist` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Ip` varchar(50) DEFAULT '0',
  `Port` int(11) DEFAULT '0',
  `Online` int(11) DEFAULT '0',
  `GroupId` int(11) DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。


-- 导出  表 sichuan.user 结构
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

-- 数据导出被取消选择。
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
