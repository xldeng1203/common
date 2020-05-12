SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for GAMEPLAYER
-- ----------------------------
DROP TABLE IF EXISTS `GAMEPLAYER`;
CREATE TABLE `GAMEPLAYER` (
  `ACCID` int(10) unsigned NOT NULL DEFAULT '0',
  `CHARID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `GUILD_NAME` varchar(33) NOT NULL DEFAULT '',
  `CREATETIME` bigint(32) unsigned DEFAULT NULL,
  `ONLINETIME` int(10) unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(48) NOT NULL DEFAULT '',
  `SEX` int(10) unsigned NOT NULL DEFAULT '0',
  `JOB` int(10) unsigned NOT NULL DEFAULT '0',
  `HAIR_STYLE` int(10) unsigned NOT NULL DEFAULT '0',
  `HAIR_COLOR` int(10) unsigned NOT NULL DEFAULT '0',
  `FACE_STYLE` int(10) unsigned NOT NULL DEFAULT '0',
  `SCENEID` int(10) unsigned NOT NULL DEFAULT '0',
  `POS_X` int(10) unsigned NOT NULL DEFAULT '0',
  `POS_Y` int(10) unsigned NOT NULL DEFAULT '0',
  `LEVEL` int(10) unsigned NOT NULL DEFAULT '0',
  `FORBID_FLAG_TALK` int(10) unsigned NOT NULL DEFAULT '0',
  `PLATFORM_ID` int(10) unsigned NOT NULL DEFAULT '0',
  `CHANNLE_ID` varchar(33) NOT NULL DEFAULT '',
  `GUILD_ID` int(10) unsigned NOT NULL DEFAULT '0',
  `NEWER_FLAG` int(1) unsigned NOT NULL DEFAULT '0',
  `HP` bigint(21) unsigned NOT NULL DEFAULT '0',
  `MP` bigint(21) unsigned NOT NULL DEFAULT '0',
  `ANGER` bigint(21) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`CHARID`),
  KEY `NAME` (`NAME`),
  KEY `ACCID` (`ACCID`),
  KEY `CHARID` (`CHARID`),
  KEY `JOB` (`JOB`)
) ENGINE=InnoDB AUTO_INCREMENT=104470 DEFAULT CHARSET=utf8;

-- 不可删除库 
CREATE TABLE `SERVER_GLOBAL_DATA` (
  `ID` int(11) NOT NULL DEFAULT '1',
  `VERSION` int(11) NOT NULL DEFAULT '10000',
  `AVERAGE_LEVEL` int(5) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of SERVER_GLOBAL_DATA
-- ----------------------------
INSERT INTO `SERVER_GLOBAL_DATA` VALUES ('1','10000', '0');
