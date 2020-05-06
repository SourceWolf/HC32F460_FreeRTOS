#ifndef CLASS_DEV_H
#define CLASS_DEV_H
#include "hc32_ddl.h"
typedef uint8_t status_device_t;
typedef uint16_t rt_size_t;
typedef uint16_t rt_off_t;
struct hc_device
{
    void    *device_id;                /**< 0 - 255 */
	void 	*config;//ÊôÐÔ
    const struct rt_device_ops *ops;
};
typedef hc_device *hc_device_t;
struct rt_device_ops
{
    /* common device interface */
	status_device_t  (*init)   (hc_device_t dev);
    status_device_t  (*open)   (hc_device_t dev, uint16_t oflag);
    status_device_t  (*close)  (hc_device_t dev);
    rt_size_t (*read)   (hc_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
    rt_size_t (*write)  (hc_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size);
    status_device_t  (*control)(hc_device_t dev, int cmd, void *args);
};
class hcDevice
{
	public:
    hcDevice();
	~hcDevice();
	status_device_t Device_regist(hc_device_t dev);
	rt_size_t Read(void *buffer,  rt_size_t size);
	rt_size_t Write(void *buffer,  rt_size_t size);
	status_device_t Close();
	status_device_t Open();
	private:
	uint16_t               flag;                     /**< device flag */
    uint16_t               open_flag;                /**< device open flag */
    uint8_t                ref_count;                /**< reference count */  
	const struct rt_device_ops *ops;
	hc_device_t dev;
};


#endif
