### Overview
IoT based on ESP8266 (NodeMCU v3) and Yandex Cloud with integration with Alice voice assistant see `terraform/cloud-functions/alice-func`


### Cloud infrastructure provisioning

note to start working with terraform you have to define `terraform/variables_yc_account.tf`
file with actual values based on the [yandex terraform provider](https://registry.terraform.io/providers/yandex-cloud/yandex/latest/docs)

1. cd `./terraform`
1. define `variables_yc_account.tf`
1. `terraform init`
1. `terraform apply`

### Arduino firmware

once you have device id and device password from the terraform output,
you need to define `arduino/esp8266-dht12-air-monitor/local_specific_variables.h`
based on the sample file `arduino/esp8266-dht12-air-monitor/local_specific_variables.h.sample`