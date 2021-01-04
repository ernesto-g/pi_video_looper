#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xe7cc3b88, "module_layout" },
	{ 0x3ce4ca6f, "disable_irq" },
	{ 0xf9a482f9, "msleep" },
	{ 0x9a152689, "param_ops_int" },
	{ 0x7fa720a6, "hrtimer_active" },
	{ 0xcc20ac52, "hrtimer_forward" },
	{ 0x4cf17d9a, "hrtimer_cancel" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x79f06713, "gpio_to_desc" },
	{ 0xb43f9365, "ktime_get" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xb8f5588b, "tty_register_driver" },
	{ 0x67ea780, "mutex_unlock" },
	{ 0x25270b7e, "put_tty_driver" },
	{ 0x8a140e9f, "tty_set_operations" },
	{ 0xe6d81d31, "hrtimer_start_range_ns" },
	{ 0xcede05bd, "__tty_insert_flip_char" },
	{ 0xe346f67a, "__mutex_init" },
	{ 0xc5850110, "printk" },
	{ 0x7c13ee85, "tty_port_init" },
	{ 0xc271c3be, "mutex_lock" },
	{ 0x847f05e2, "gpiod_direction_input" },
	{ 0xde71008, "gpiod_direction_output_raw" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0xced9e687, "gpiod_set_debounce" },
	{ 0x67b27ec1, "tty_std_termios" },
	{ 0x7e95e504, "tty_unregister_driver" },
	{ 0x71a0f97d, "__tty_alloc_driver" },
	{ 0xfe990052, "gpio_free" },
	{ 0x409873e3, "tty_termios_baud_rate" },
	{ 0xfcec0987, "enable_irq" },
	{ 0xf5214bdb, "tty_port_link_device" },
	{ 0xd3d255e8, "gpiod_to_irq" },
	{ 0x51389e35, "gpiod_set_raw_value" },
	{ 0x18ddfa86, "hrtimer_init" },
	{ 0x55f3e465, "tty_flip_buffer_push" },
	{ 0xd405fa50, "gpiod_get_raw_value" },
	{ 0xc1514a3b, "free_irq" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "CE906AE4009F66B2BDB88E2");
