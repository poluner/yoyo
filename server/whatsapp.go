// whatsapp status 分享页面
package server


func (p *listParam) WhatsApp() (result []WhatsApp, err error) {
	cmdResult := redisConn.LRange(
		whatsAppRedisList, int64(p.Offset), int64(p.Offset + p.Limit))
	resourceIds := cmdResult.Val()
	result, err = QueryWhatsapp(p.ctx, resourceIds)
	return
}
