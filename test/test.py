import unittest
import requests
import http.client
import aiohttp
import asyncio

t_cookies = {
    '_ga': 'GA1.1.1853335373.1693823153',
    '_ga_M78MKT6JKY': 'GS1.1.1693823153.1.1.1693823965.0.0.0',
    '_ga_XNTP0G5VDT': 'GS1.1.1696850402.1.1.1696853451.0.0.0',
}

t_headers = {
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

server0 = "http://localhost:8081"
server1 = "http://localhost:8082"
server2 = "http://localhost:8083"
base_url = "localhost:8081"
base_url1 = "localhost:8082"
base_url2 = "localhost:8083"

############################
# RESPONSE TESTS
############################
class TestServerResponse(unittest.TestCase):
    ############################
    # GET TESTS
    ############################
    def test_get(self):
        url = server0
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 200)

    def test_get_not_found(self):
        url = f"{server0}/doesntexist"
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 404)

    def test_get_method_not_allowed(self):
        url = f"{server0}/no-get"
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 405)

    def test_get_autoindex(self):
        url = f"{server0}/auto-index/"
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 200)

    ############################
    # POST TESTS
    ############################
    def test_post(self):
        url = f"{server0}/upload"
        files = {'file': open('test/Poster.jpg', 'rb')}
        headers = {'Content-Length': '1046495'}

        response = requests.post(url=url, headers=headers, files=files)
        files["file"].close()
        self.assertTrue(response.history)
        self.assertEqual(response.history[0].status_code, 303)

    def test_post_bigger_max_client_size(self):
        url = f"{server0}/max-post"
        files = {'file': open('test/Poster.jpg', 'rb')}
        headers = {'Content-Length': '1046495'}

        response = requests.post(url=url, headers=headers, files=files)
        files["file"].close()
        self.assertEqual(response.status_code, 413)

    def test_post_method_not_allowed(self):
        url = f"{server0}/no-post"
        files = {'file': open('test/Poster.jpg', 'rb')}
        headers = {'Content-Length': '1046495'}

        response = requests.post(url=url, headers=headers, files=files)
        files["file"].close()
        self.assertEqual(response.status_code, 405)

    def test_post_chunked(self):
        conn = http.client.HTTPConnection(base_url)
        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"7\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 200)

    def test_post_invalid_chunks(self):
        conn = http.client.HTTPConnection(base_url)
        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"9\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 500)

    def test_post_empty_chunk(self):
        conn = http.client.HTTPConnection(base_url)
        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"0\r\n\r\n")
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 500)

    ############################
    # DELETE TESTS
    ############################
    def test_delete(self):
        filename = 'test.txt'
        file_path = f"test/www-test/delete/{filename}"
        with open(file_path, 'w') as file:
            file.write("Test content")

        url = f"{server0}/delete/{filename}"
        response = requests.delete(url=url)
        self.assertEqual(response.status_code, 200)

    def test_delete_method_not_allowed(self):
        url = f"{server0}/no-delete/test"
        response = requests.delete(url=url)
        self.assertEqual(response.status_code, 405)

    def test_delete_file_does_not_exist(self):
        url = f"{server0}/delete/does_not_exist"
        response = requests.delete(url=url)
        self.assertEqual(response.status_code, 404)

    ############################
    # UNKNOWN TESTS
    ############################
    def test_unknown_method(self):
        conn = http.client.HTTPConnection(base_url)
        conn.putrequest("QWERTY", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"9\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 405)

    ############################
    # CGI TESTS
    ############################
    def cgi_post_get(self):
        response = requests.post(
            f"{server0}/cgi-bin", cookies=t_cookies, headers=t_headers)
        # print(response.status_code)
        self.assertEqual(response.status_code, 200)
        response = requests.get(
            f"{server0}/cgi-bin/post", cookies=t_cookies, headers=t_headers)
        self.assertEqual(response.status_code, 500)
        # print(response.status_code)

    def cgi_error_files(self):
        response = requests.post(
            f"{server2}/cgi-bin/error", cookies=t_cookies, headers=t_headers) # loop
        self.assertEqual(response.status_code, 502)
        # print(response.status_code)
        response = requests.get(
            f"{server2}/cgi-bin/loop", cookies=t_cookies, headers=t_headers) # error
        self.assertEqual(response.status_code, 504)
        # print(response.status_code)

    ############################
    # SPAM TESTS
    ############################
    def test_spam_get(self):
        for _ in range(5):
            response = requests.get(
                server0, cookies=t_cookies, headers=t_headers)
            # print(response.status_code)
            self.assertEqual(response.status_code, 200)

    def test_spam_get_3(self):
        for _ in range(5):
            response = requests.get(
                server0, cookies=t_cookies, headers=t_headers)
            # print(response.status_code)
            self.assertEqual(response.status_code, 200)
            response = requests.get(
                server1, cookies=t_cookies, headers=t_headers)
            # print(response.status_code)
            self.assertEqual(response.status_code, 200)
            response = requests.get(
                server2, cookies=t_cookies, headers=t_headers)
            # print(response.status_code)
            self.assertEqual(response.status_code, 200)

    def post_request_template(self, base_url):
        conn = http.client.HTTPConnection(base_url)
        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"7\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")
        response = conn.getresponse()
        conn.close()
        return response
    
    def test_spam_post(self):
        for _ in range(5):
            response = self.post_request_template(base_url)
            self.assertEqual(response.status, 200)

    def test_spam_post_3(self):
        for _ in range(5):
            response = self.post_request_template(base_url)
            # print(response.status_code)
            self.assertEqual(response.status, 200)
            response = self.post_request_template(base_url1)
            # print(response.status_code)
            self.assertEqual(response.status, 200)
            response = self.post_request_template(base_url2)
            # print(response.status_code)
            self.assertEqual(response.status, 200)

    # def test_spam_cgi_post(self):
    #     for _ in range(5):
    #         response = requests.post(
    #             f"{server0}/cgi-bin", cookies=t_cookies, headers=t_headers)
    #         # print(response.status_code)
    #         self.assertEqual(response.status_code, 200)

    # def test_spam_cgi_post_3(self):
    #     for _ in range(5):
    #         response = requests.post(
    #             f"{server0}/cgi-bin", cookies=t_cookies, headers=t_headers)
    #         # print(response.status_code)
    #         self.assertEqual(response.status_code, 200)
    #         response = requests.post(
    #             f"{server1}/cgi-bin", cookies=t_cookies, headers=t_headers)
    #         # print(response.status_code)
    #         self.assertEqual(response.status_code, 200)
    #         response = requests.post(
    #             f"{server2}/cgi-bin", cookies=t_cookies, headers=t_headers)
    #         # print(response.status_code)
    #         self.assertEqual(response.status_code, 200)

############################
# ASYNC TESTS
############################
async def make_async_request(session, method, url):
    async with session.request(method, url, cookies=t_cookies, headers=t_headers) as response:
        status = response.status
        print(status)

async def async_main():
    url_post = f"{server0}/cgi-bin"
    url_get = f"{server0}/cgi-bin/get?name=value"

    async with aiohttp.ClientSession() as session:
        post_tasks = [make_async_request(session, 'POST', url_post) for _ in range(10)]
        # get_tasks = [make_async_request(session, 'GET', url_get) for _ in range(10)]

        await asyncio.gather(*post_tasks, *get_tasks)

############################
# CGI TEST
############################
# def cgi_post_get():
#     response = requests.post(
#         f"{server0}/cgi-bin", cookies=t_cookies, headers=t_headers)
#     print(response.status_code)
#     response = requests.get(
#         f"{server0}/cgi-bin/post", cookies=t_cookies, headers=t_headers)
#     print(response.status_code)

# def cgi_error_files():
#     response = requests.post(
#         f"{server2}/cgi-bin/error", cookies=t_cookies, headers=t_headers) # loop
#     print(response.status_code)
#     response = requests.get(
#         f"{server2}/cgi-bin/loop", cookies=t_cookies, headers=t_headers) # error
#     print(response.status_code)

############################
# MAIN
############################
if __name__ == '__main__':

    # Async tests
    # print("\nASYNC tests")
    # asyncio.run(async_main())

    # CGI tests
    # print("\nCGI tests")
    # cgi_post_get()
    # cgi_error_files()

    # Response tests
    print("\nRESPONSE tests")
    unittest.main()
