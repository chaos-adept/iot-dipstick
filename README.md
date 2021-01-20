### Overview
IoT based on ESP8266 ([NodeMCU v3 board](http://www.nodemcu.com/index_cn.html)) and [Yandex Cloud](https://cloud.yandex.ru/) 
with integration with [Alice](https://yandex.ru/dev/dialogs/alice/doc/about.html)  assistant see [alice-func](terraform/cloud-functions/alice-func)

Alice configuration can't be provided in the source code, you have to configure it on your own. See references in the end of this page.

### Cloud infrastructure provisioning

note to start working with terraform you have to make a `terraform/variables_yc_account.tf`
file with actual values based on the [yandex terraform provider](https://registry.terraform.io/providers/yandex-cloud/yandex/latest/docs)

1. cd `./terraform`
1. make `variables_yc_account.tf` based on [variables_yc_account.tf.sample](terraform/variables_yc_account.tf)
1. `terraform init`
1. `terraform apply`

### Arduino firmware

once you have device id and device password from the terraform output,
you need to define `arduino/esp8266-dht12-air-monitor/local_specific_variables.h`
based on the sample file [arduino/esp8266-dht12-air-monitor/local_specific_variables.h.sample](arduino/esp8266-dht12-air-monitor/local_specific_variables.h.sample)

### Resources to get started with the project
* [Before to Get Started](../../wiki/Before-to-Get-Started) - List of resources to get required knowledge to work with the project might be found in the wiki page
* [Prehistory and Architecture notes](../../wiki/Prehistory-and-Architecture-notes) - Prehistory of an approach might be found here
* [Alice abilities overview](https://yandex.ru/dev/dialogs/alice/doc/about.html) - Alice abilities development get startred
* [Alice based on the cloud function](https://yandex.ru/dev/dialogs/alice/doc/deploy-ycloud-function.html) - How to integrate [alice-func](terraform/cloud-functions/alice-func) into Alice ability
