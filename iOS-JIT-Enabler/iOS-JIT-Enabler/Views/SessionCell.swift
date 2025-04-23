import UIKit

class SessionCell: UITableViewCell {
    static let identifier = "SessionCell"
    
    // UI Elements
    private let appNameLabel: UILabel = {
        let label = UILabel()
        label.font = UIFont.systemFont(ofSize: 16, weight: .medium)
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    private let bundleIdLabel: UILabel = {
        let label = UILabel()
        label.font = UIFont.systemFont(ofSize: 12)
        label.textColor = .gray
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    private let statusLabel: UILabel = {
        let label = UILabel()
        label.font = UIFont.systemFont(ofSize: 14, weight: .medium)
        label.textAlignment = .right
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    private let timestampLabel: UILabel = {
        let label = UILabel()
        label.font = UIFont.systemFont(ofSize: 12)
        label.textColor = .gray
        label.textAlignment = .right
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    // MARK: - Initialization
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupUI()
    }
    
    // MARK: - UI Setup
    
    private func setupUI() {
        // Add subviews
        contentView.addSubview(appNameLabel)
        contentView.addSubview(bundleIdLabel)
        contentView.addSubview(statusLabel)
        contentView.addSubview(timestampLabel)
        
        // Set up constraints
        NSLayoutConstraint.activate([
            // App name constraints
            appNameLabel.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 16),
            appNameLabel.topAnchor.constraint(equalTo: contentView.topAnchor, constant: 12),
            appNameLabel.trailingAnchor.constraint(equalTo: statusLabel.leadingAnchor, constant: -8),
            
            // Bundle ID constraints
            bundleIdLabel.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 16),
            bundleIdLabel.topAnchor.constraint(equalTo: appNameLabel.bottomAnchor, constant: 4),
            bundleIdLabel.trailingAnchor.constraint(equalTo: statusLabel.leadingAnchor, constant: -8),
            bundleIdLabel.bottomAnchor.constraint(lessThanOrEqualTo: contentView.bottomAnchor, constant: -12),
            
            // Status label constraints
            statusLabel.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -16),
            statusLabel.topAnchor.constraint(equalTo: contentView.topAnchor, constant: 12),
            statusLabel.widthAnchor.constraint(lessThanOrEqualToConstant: 120),
            
            // Timestamp label constraints
            timestampLabel.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -16),
            timestampLabel.topAnchor.constraint(equalTo: statusLabel.bottomAnchor, constant: 4),
            timestampLabel.widthAnchor.constraint(lessThanOrEqualToConstant: 120),
            timestampLabel.bottomAnchor.constraint(lessThanOrEqualTo: contentView.bottomAnchor, constant: -12)
        ])
    }
    
    // MARK: - Configuration
    
    func configure(with session: Session) {
        appNameLabel.text = session.appName
        bundleIdLabel.text = session.bundleId
        timestampLabel.text = session.formattedStartTime
        
        // Set status label text and color
        statusLabel.text = session.statusDisplayText
        
        switch session.status {
        case "completed":
            statusLabel.textColor = .systemGreen
        case "processing":
            statusLabel.textColor = .systemOrange
        case "failed":
            statusLabel.textColor = .systemRed
        case "expired":
            statusLabel.textColor = .systemGray
        case "active":
            statusLabel.textColor = .systemBlue
        default:
            statusLabel.textColor = .label
        }
    }
    
    override func prepareForReuse() {
        super.prepareForReuse()
        appNameLabel.text = nil
        bundleIdLabel.text = nil
        statusLabel.text = nil
        timestampLabel.text = nil
        statusLabel.textColor = .label
    }
}