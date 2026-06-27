$ErrorActionPreference = "Stop"

try {
    $client = New-Object System.Net.Sockets.TcpClient
    $client.Connect("127.0.0.1", 9876)
    Write-Host "=== TCP Connected! ===" -ForegroundColor Green

    $stream = $client.GetStream()
    $writer = New-Object System.IO.StreamWriter($stream)
    $reader = New-Object System.IO.StreamReader($stream)
    $writer.AutoFlush = $true

    # Query initial state
    $writer.WriteLine("GET_DATA")
    Start-Sleep -Milliseconds 100
    $resp = $reader.ReadLine()
    Write-Host "GET_DATA: $resp"

    # Open valve
    $writer.WriteLine("OPEN")
    Start-Sleep -Milliseconds 100
    $resp = $reader.ReadLine()
    Write-Host "OPEN: $resp"

    # Poll 5 times
    for ($i = 1; $i -le 5; $i++) {
        Start-Sleep -Milliseconds 300
        $writer.WriteLine("GET_DATA")
        $resp = $reader.ReadLine()
        Write-Host "[$i] $resp"
    }

    # Stop
    $writer.WriteLine("STOP")
    Start-Sleep -Milliseconds 100
    $resp = $reader.ReadLine()
    Write-Host "STOP: $resp"

    $client.Close()
    Write-Host "=== Test Complete ===" -ForegroundColor Green
} catch {
    Write-Host "FAILED: $($_.Exception.Message)" -ForegroundColor Red
}
