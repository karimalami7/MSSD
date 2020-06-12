# author: Karim Alami
#
# created: 22/03/2020
#
# Simple Kafka producer for twitter use case

from kafka import KafkaProducer
import time

producer = KafkaProducer(bootstrap_servers='localhost:9092')

# def on_send_success(record_metadata):
#     print(record_metadata.topic)
#     print(record_metadata.partition)
#     print(record_metadata.offset)

def on_send_error(excp):
    log.error('error', exc_info=excp)

with open("../twitter_api_pipeline/tweets.txt","r") as t:

	tweet=t.readlines()
	counter=0
	batch=1000
	while counter<len(tweet):
		print("streaming", counter," -->", counter+batch-1)
		for i in range(counter, counter+batch):
			producer.send('twitter_topic', tweet[i].strip().encode('utf-8')).add_errback(on_send_error)
		counter+=batch
		time.sleep(5)