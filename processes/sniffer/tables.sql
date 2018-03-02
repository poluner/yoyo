CREATE SCHEMA `yoyo` DEFAULT CHARACTER SET utf8mb4 ;

CREATE TABLE `yoyo`.`infohash_task` (
  `id` BIGINT NOT NULL AUTO_INCREMENT COMMENT '主键id',
  `infohash` VARCHAR(40) NOT NULL COMMENT 'bt infohash',
  `get_peer` INT NOT NULL DEFAULT 0 COMMENT 'get_peer 次数',
  `announce_peer` INT NOT NULL DEFAULT 0 COMMENT 'announce_per 次数',
  `status` TINYINT NOT NULL DEFAULT 0 COMMENT '状态 0 正在查询 1 metadata查询成功 2 metadata查询失败',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最近修改时间',
  PRIMARY KEY (`id`),
  INDEX `ix_infohash` (`infohash`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4
COMMENT = 'infohash表';
