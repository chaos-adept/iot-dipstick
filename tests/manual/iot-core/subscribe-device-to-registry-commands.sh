#fixme add asserts that env variables are defined

#REGISTRY_ID="areiqldr2chj5v0g2rd6"
#REGISTRY_PASSWORD="zx+z{i1QD6YJ#J#&"
#DEVICE_ID="arekifdlttq12q6sslle"
#DEVICE_PASSWORD="HsTvGAN6HFundl1v"


yc iot mqtt subscribe \
--username "${DEVICE_ID}" \
--password "${DEVICE_PASSWORD}" \
--topic "\$registries/${REGISTRY_ID}/commands" \
--qos 1