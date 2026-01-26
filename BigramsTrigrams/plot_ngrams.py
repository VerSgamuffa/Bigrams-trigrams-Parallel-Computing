import pandas as pd
import matplotlib.pyplot as plt
import sys

#use the filename passed from the terminal
csv_file = sys.argv[1] if len(sys.argv) > 1 else "character_ngrams.csv"
#csv_file = sys.argv[1] if len(sys.argv) > 1 else "word_ngrams.csv"

df = pd.read_csv(csv_file, encoding="latin1")

#sort by frequency
df = df.sort_values("count", ascending=False)

#top 30 most frequent
df = df.head(30)

plt.figure(figsize=(16, 8))
plt.bar(df["ngram"], df["count"])
plt.xticks(rotation=75, ha='right')
plt.xlabel("n-gram")
plt.ylabel("Occorrenze")
plt.title(f"Istogramma 3-gram da {csv_file}")

plt.tight_layout()

output = csv_file.replace(".csv", ".png")
plt.savefig(output, dpi=300)
print("Creato grafico:", output)
