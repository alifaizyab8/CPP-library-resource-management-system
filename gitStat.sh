#!/bin/bash

# Configuration
AUTHORS=("alifaizyab8" "r2meen" "Bareerah22" "waqarwasif")

# Arrays to store stats
declare -A stats
total_commits=0

echo "Calculating statistics..."

# Collect data for each author
for author in "${AUTHORS[@]}"; do
    # Get commits
    commits=$(git log --author="$author" --all --no-merges --oneline | wc -l)
    
    # Get insertions and deletions
    read insertions deletions < <(git log --author="$author" --all --no-merges --pretty=tformat: --numstat | \
        awk '$1 != "-" && $2 != "-" { ins += $1; del += $2 } END { print ins+0, del+0 }')
    
    # Store stats
    stats["${author}_commits"]=$commits
    stats["${author}_insertions"]=$insertions
    stats["${author}_deletions"]=$deletions
    
    total_commits=$((total_commits + commits))
done

# Sort authors by commits (descending)
IFS=$'\n' ranked_authors=($(
    for author in "${AUTHORS[@]}"; do
        echo "${stats[${author}_commits]} $author"
    done | sort -rn | cut -d' ' -f2-
))

# Print leaderboard
echo "------------------------------------------------------------------------"
echo "                        🏆 LEADERBOARD 🏆"
echo "------------------------------------------------------------------------"
printf "%-5s %-15s %-10s %-12s %-12s %-15s\n" "Rank" "Author" "Commits" "Insertions" "Deletions" "Contribution %"
echo "------------------------------------------------------------------------"

rank=1
for author in "${ranked_authors[@]}"; do
    commits=${stats[${author}_commits]}
    insertions=${stats[${author}_insertions]}
    deletions=${stats[${author}_deletions]}
    
    # Calculate percentage
    if [ "$total_commits" -gt 0 ]; then
        percent=$(awk "BEGIN {printf \"%.1f\", ($commits * 100) / $total_commits}")
    else
        percent="0.0"
    fi
    
    # Medal for top 3
    medal=""
    case $rank in
        1) medal="🥇" ;;
        2) medal="🥈" ;;
        3) medal="🥉" ;;
        4) medal="🏆" ;;
    esac
    
    printf "%-5s %-15s %-10s %-12s %-12s %-15s\n" "$medal#$rank" "$author" "$commits" "$insertions" "$deletions" "$percent%"
    rank=$((rank + 1))
done

echo "------------------------------------------------------------------------"
echo "Total Commits: $total_commits"
echo "------------------------------------------------------------------------"