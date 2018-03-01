CREATE SCHEMA `yoyo` DEFAULT CHARACTER SET utf8mb4 ;

CREATE TABLE `yoyo`.`infohash_task` (
  `id` BIGINT NOT NULL AUTO_INCREMENT COMMENT '主键id',
  `infohash` VARCHAR(40) NOT NULL COMMENT 'bt infohash',
  `hot` INT NOT NULL DEFAULT 0 COMMENT '热门指数',
  `status` TINYINT NOT NULL DEFAULT 0 COMMENT '状态 1metainfo插入es成功 2metainfo插入es失败',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最近修改时间',
  PRIMARY KEY (`id`),
  INDEX `ix_infohash` (`infohash`),
  INDEX `ix_status_updated` (`status`, `updated_at`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4
COMMENT = 'infohash表';
