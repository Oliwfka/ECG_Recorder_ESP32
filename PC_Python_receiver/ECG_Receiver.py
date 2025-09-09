import simplepyble
import time
import struct

def save_results_to_file(ecg_value, acc_x_value, acc_y_value, acc_z_value, gyro_x_value, gyro_y_value, gyro_z_value):
    with open(name+".txt", "a") as file:
        if file.tell() == 0:
            file.write("EKG [uV], Acceleration axis x [m^2/s], Acceleration axis y [m^2/s], Acceleration axis z [m^2/s], Angular velocity axis x [degree/s], Angular velocity axis y [degree/s], Angular velocity axis z [degree/s]\n")

        file.write(f"{ecg_value}, {acc_x_value}, {acc_y_value}, {acc_z_value}, {gyro_x_value}, {gyro_y_value}, {gyro_z_value}\n")


if __name__ == "__main__":
    adapters = simplepyble.Adapter.get_adapters()

    if len(adapters) == 0:
        print("No adapters found")

    # Query the user to pick an adapter
    print("Please select an adapter:")
    for i, adapter in enumerate(adapters):
        print(f"{i}: {adapter.identifier()} [{adapter.address()}]")

    choice = int(input("Enter choice: "))
    adapter = adapters[choice]

    print(f"Selected adapter: {adapter.identifier()} [{adapter.address()}]")

    adapter.set_callback_on_scan_start(lambda: print("Scan started."))
    adapter.set_callback_on_scan_stop(lambda: print("Scan complete."))
    adapter.set_callback_on_scan_found(lambda peripheral: print(f"Found {peripheral.identifier()} [{peripheral.address()}]"))

    # Scan for 5 seconds
    adapter.scan_for(5000)
    peripherals = adapter.scan_get_results()
    

    # Query the user to pick a peripheral
    print("Please select a peripheral:")
    for i, peripheral in enumerate(peripherals):
        print(f"{i}: {peripheral.identifier()} [{peripheral.address()}]")
        if peripheral.identifier() == "EKG":
            choice = i

    #choice = int(input("Enter choice: "))
    peripheral = peripherals[choice]

    print(f"Connecting to: {peripheral.identifier()} [{peripheral.address()}]")
    peripheral.connect()

            
    service_uuid = "53d1aafe-ddef-4165-bf9e-4d06efac243c"
    acc_x_uuid = "8a611f55-6639-4b7c-ad98-30b59252692e"
    acc_y_uuid = "5921c426-8576-45f1-aa6a-67677bc76d35"
    acc_z_uuid = "620da595-293a-4f28-a52b-d24b9ead503b"
    gyro_x_uuid = "a0f90571-4f82-4cf6-b502-228bb5c4090b"
    gyro_y_uuid = "2b50ea61-f47e-4bb6-a352-bab845c4d6a5"
    gyro_z_uuid = "9d3ca539-6f48-4c61-be81-1b97d65d16e9"
    ecg_uuid = "ef3d9410-d257-4693-a1eb-63e89274a848"

    name = input("Enter file name: ")

        

    # Write the content to the characteristic
    #contents_ecg = peripheral.notify(service_uuid, ecg_uuid, ecg_notification)
    #contents_acc = peripheral.notify(service_uuid, acc_x_uuid, acc_notification)
    #contents_gyro = peripheral.notify(service_uuid, gyro_x_uuid, gyro_notification)

    while (True):

        contents_ecg = struct.unpack('d',peripheral.read(service_uuid, ecg_uuid))[0]
        contents_acc_x = struct.unpack('d',peripheral.read(service_uuid, acc_x_uuid))[0]
        contents_acc_y = struct.unpack('d',peripheral.read(service_uuid, acc_y_uuid))[0]
        contents_acc_z = struct.unpack('d',peripheral.read(service_uuid, acc_z_uuid))[0]
        contents_gyro_x = struct.unpack('d',peripheral.read(service_uuid, gyro_x_uuid))[0]
        contents_gyro_y = struct.unpack('d',peripheral.read(service_uuid, gyro_y_uuid))[0]
        contents_gyro_z = struct.unpack('d',peripheral.read(service_uuid, gyro_z_uuid))[0]
        print(f"ECG (mV): {contents_ecg}\t")
        print(f"Acceleration (m^2/s): {contents_acc_x}\t")
        print(f"Angular velocity (degree/s): {contents_gyro_x}\n")

        save_results_to_file(contents_ecg, contents_acc_x, contents_acc_y, contents_acc_z, contents_gyro_x, contents_gyro_y, contents_gyro_z)

    time.sleep(0.001)

    peripheral.disconnect()

