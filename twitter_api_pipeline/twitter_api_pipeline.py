from tweepy.streaming import StreamListener
from tweepy import OAuthHandler
from tweepy import Stream
import json
import socket
import twitter_api_credentials
 

class TwitterAuthenticator():

    def authenticate_twitter_app(self):
        auth = OAuthHandler(twitter_api_credentials.CONSUMER_KEY, twitter_api_credentials.CONSUMER_SECRET)
        auth.set_access_token(twitter_api_credentials.ACCESS_TOKEN, twitter_api_credentials.ACCESS_SECRET)
        return auth



class TwitterListenerAndSender(StreamListener):
    """
    This gets tweets from the stream, process the interesting attributes, and send them to Spark through Tcp connection
    """
    def __init__(self, conn):
        self.conn = conn

    def on_data(self, data):
        try:
            tweet_in_json=json.loads(data)
            
            #attributes to analyze tweets
            tweet_id=tweet_in_json["id"]
            user_id=tweet_in_json["user"]["id"]
            user_followers_count=tweet_in_json["user"]["followers_count"]
            user_friends_count=tweet_in_json["user"]["friends_count"]
            user_listed_count=tweet_in_json["user"]["listed_count"]
            user_favourites_count=tweet_in_json["user"]["favourites_count"]
            user_statuses_count=tweet_in_json["user"]["statuses_count"]

            tweet_data=str(tweet_id)+","+str(user_id)+","+str(user_followers_count)+","+str(user_friends_count)+","+\
                str(user_listed_count)+","+str(user_favourites_count)+","+str(user_statuses_count)

            with open("tweets.txt","a") as t:
                t.write(tweet_data+"\n")

            conn.send((tweet_data+"\n").encode("utf-8"))
            return True
        except BaseException as e:
            print("Error on_data %s" % str(e))
        return True
          

    def on_error(self, status):
        print(status)


class TwitterStreamer():
    """
    Class for streaming and processing live tweets.
    """
    def __init__(self):
        pass

    def stream_tweets(self, conn, hash_tag_list):
        # This handles Twitter authetification and the connection to Twitter Streaming API
        listener = TwitterListenerAndSender(conn)
        auth = TwitterAuthenticator().authenticate_twitter_app()
        stream = Stream(auth, listener)

        # This line filter Twitter Streams to capture data by the keywords: 
        stream.filter(track=hash_tag_list)

class TcpSender():
    """
    Class for opening a Tcp connection.
    """
    def __init__(self):
        self.TCP_IP = "localhost"
        self.TCP_PORT = 9010

    def open_connection(self):
        conn = None
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((self.TCP_IP, self.TCP_PORT))
        s.listen(1)
        print("Waiting for TCP connection on ",self.TCP_PORT)
        conn, addr = s.accept()
        print("Connected on "+ str(addr) +"... Starting getting tweets.")
        return conn
 
if __name__ == '__main__':
 
    tcp_sender=TcpSender()
    conn=tcp_sender.open_connection()

    twitter_streamer = TwitterStreamer()
    hash_tag_list = ['#','@']
    twitter_streamer.stream_tweets(conn, hash_tag_list)

