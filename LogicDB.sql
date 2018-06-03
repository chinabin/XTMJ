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

-- 导出 sichuan_majiang 的数据库结构
CREATE DATABASE IF NOT EXISTS `mj_logic` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `mj_logic`;


-- 导出  表 sichuan_majiang.activelog 结构
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

CREATE TABLE `activitylog` (
	`AutoId` INT(11) NOT NULL AUTO_INCREMENT,
	`Id` INT(11) NOT NULL,
	`UserId` INT(11) NOT NULL DEFAULT '0',
	`SpendType` INT(11) NOT NULL DEFAULT '0',
	`SpendCount` INT(11) NOT NULL DEFAULT '0',
	`GiftName` VARCHAR(128) NOT NULL DEFAULT '',
	`GiftType` INT(11) NOT NULL DEFAULT '0',
	`GiftCount` INT(11) NOT NULL DEFAULT '0',
	`Date` INT(11) NOT NULL DEFAULT '0',
	`Phone` VARCHAR(32) NOT NULL DEFAULT '',
	PRIMARY KEY (`AutoId`),
	KEY `Id` (`Id`),
	KEY `UserId` (`UserId`),
	KEY `Date` (`Date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- 导出  表 sichuan_majiang.bindingrelationships 结构
CREATE TABLE IF NOT EXISTS `bindingrelationships` (
  `Invitee` int(11) NOT NULL,
  `Inviter` int(11) NOT NULL,
  `BindingTime` int(11) DEFAULT NULL,
  `TaskFinished` int(11) DEFAULT NULL,
  PRIMARY KEY (`Invitee`,`Inviter`),
  KEY `Inviter` (`Inviter`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 数据导出被取消选择。


-- 导出  表 sichuan_majiang.log 结构
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

-- 导出  表 sichuan_majiang.log 结构
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


-- 导出  表 sichuan_majiang.video 结构
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

-- 导出  表 sichuan_majiang.video 结构
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
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
