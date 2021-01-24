#fixme add asserts that env variables are defined

#REGISTRY_ID="areiqldr2chj5v0g2rd6"
#REGISTRY_PASSWORD="zx+z{i1QD6YJ#J#&"
#DEVICE_ID="arekifdlttq12q6sslle"
#DEVICE_PASSWORD="HsTvGAN6HFundl1v"


yc iot mqtt publish \
--username "${REGISTRY_ID}" \
--password "${REGISTRY_PASSWORD}" \
--topic "\$registries/${REGISTRY_ID}/commands" \
--message 'Test data' \
--qos 1