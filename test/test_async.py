import aiohttp
import asyncio

headers = {
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7',
    'Accept-Language': 'nl-NL,nl;q=0.9,en-US;q=0.8,en;q=0.7',
    'Cache-Control': 'no-cache',
    'Connection': 'keep-alive',
    'Content-Type': 'application/x-www-form-urlencoded',
    'DNT': '1',
    'Origin': 'http://localhost:8081',
    'Pragma': 'no-cache',
    'Referer': 'http://localhost:8081/',
    'Sec-Fetch-Dest': 'document',
    'Sec-Fetch-Mode': 'navigate',
    'Sec-Fetch-Site': 'same-origin',
    'Sec-Fetch-User': '?1',
    'Upgrade-Insecure-Requests': '1',
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36',
    'sec-ch-ua': '"Google Chrome";v="117", "Not;A=Brand";v="8", "Chromium";v="117"',
    'sec-ch-ua-mobile': '?0',
    'sec-ch-ua-platform': '"macOS"',
}
data = {
    'name': 'fgddfgdfg',
}


async def make_request(session, url):
    async with session.post(url, data=data, headers=headers) as response:
        status = response.status
        # Depending on the response's content type, you can use .text(), .json(), etc.
        # For example, to print the response's text: text = await response.text()
        print(status)


async def main():
    url = 'http://localhost:8081/cgi-bin/post'
    async with aiohttp.ClientSession() as session:
        tasks = [make_request(session, url) for i in range(10)]
        await asyncio.gather(*tasks)

# This is the asyncio equivalent of 'if __name__ == "__main__":'
# It will run the main() function asynchronously.
asyncio.run(main())