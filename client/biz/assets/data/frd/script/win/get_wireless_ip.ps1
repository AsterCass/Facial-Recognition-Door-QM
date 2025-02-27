$adapters = Get-NetAdapter | Where-Object { $_.Status -eq 'Up' }
foreach ($adapter in $adapters) {
    if ($adapter.InterfaceDescription -like "*Wi-Fi*") {
        $thisIp = (Get-NetIPAddress -InterfaceIndex $adapter.InterfaceIndex -AddressFamily IPv4).IPAddress
        Write-Output "$thisIp"
        return
    }
}
Write-Output ""
