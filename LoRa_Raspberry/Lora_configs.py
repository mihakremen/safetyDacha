def on_rx_done(self):
    BOARD.led_on()
    # print("\nRxDone")
    self.clear_irq_flags(RxDone=1)
    payload = self.read_payload(nocheck=True)  # Receive INF
    print("Receive: ")
    mens = bytes(payload).decode("utf-8", 'ignore')
    mens = mens[2:-1]  # to discard \x00\x00 and \x00 at the end
    print(mens)
    mosquitto.send_mqtt("safetyDacha/Device_1", mens)
    BOARD.led_off()
    if mens == "INF":
        print("Received data request INF")
        time.sleep(2)
        print("Send mens: DATA RASPBERRY PI")
        self.write_payload([255, 255, 0, 0, 68, 65, 84, 65, 32, 82, 65, 83, 80, 66, 69, 82, 82, 89, 32, 80, 73,
                            0])  # Send DATA RASPBERRY PI
        self.set_mode(MODE.TX)
    time.sleep(2)
    self.reset_ptr_rx()
    self.set_mode(MODE.RXCONT)